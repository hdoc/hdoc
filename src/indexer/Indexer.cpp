// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include <filesystem>

#include "spdlog/spdlog.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/JSONCompilationDatabase.h"
#include "clang/Tooling/Tooling.h"

#include "indexer/Indexer.hpp"
#include "indexer/Matchers.hpp"
#include "support/ParallelExecutor.hpp"

// Check if a symbol is a child of the given namespace
static bool isChild(const hdoc::types::Symbol& ns, const hdoc::types::Symbol& s) {
  return s.parentNamespaceID.raw() == ns.ID.raw();
}

void hdoc::indexer::Indexer::run() {
  spdlog::info("Starting indexing...");

  std::string err;
  const auto  stx = clang::tooling::JSONCommandLineSyntax::AutoDetect;
  const auto  cmpdb =
      clang::tooling::JSONCompilationDatabase::loadFromFile(this->cfg->compileCommandsJSON.string(), err, stx);

  if (cmpdb == nullptr) {
    spdlog::error("Unable to initialize compilation database ({})", err);
    return;
  }

  hdoc::indexer::matchers::FunctionMatcher  FunctionFinder(&this->index, this->cfg);
  hdoc::indexer::matchers::RecordMatcher    RecordFinder(&this->index, this->cfg);
  hdoc::indexer::matchers::EnumMatcher      EnumFinder(&this->index, this->cfg);
  hdoc::indexer::matchers::NamespaceMatcher NamespaceFinder(&this->index, this->cfg);
  clang::ast_matchers::MatchFinder          Finder;
  Finder.addMatcher(FunctionFinder.getMatcher(), &FunctionFinder);
  Finder.addMatcher(RecordFinder.getMatcher(), &RecordFinder);
  Finder.addMatcher(EnumFinder.getMatcher(), &EnumFinder);
  Finder.addMatcher(NamespaceFinder.getMatcher(), &NamespaceFinder);

  // Add include search paths to clang invocation
  std::vector<std::string> includePaths = {};
  for (const std::string& d : cfg->includePaths) {
    // Ignore include paths that don't exist
    if (!std::filesystem::exists(d)) {
      spdlog::warn("Include path {} does not exist. Proceeding without it.", d);
      continue;
    }
    spdlog::info("Appending {} to list of include paths.", d);
    includePaths.emplace_back("-isystem" + d);
  }

  hdoc::indexer::ParallelExecutor tool(*cmpdb, includePaths, this->pool, this->cfg->debugLimitNumIndexedFiles);
  tool.execute(clang::tooling::newFrontendActionFactory(&Finder));
}

void hdoc::indexer::Indexer::resolveNamespaces() {
  spdlog::info("Indexer resolving namespaces.");
  for (auto& [k, ns] : this->index.namespaces.entries) {
    // Add all the direct children of this namespace to its children vector
    for (const auto& [k, v] : this->index.records.entries) {
      if (isChild(ns, v)) {
        ns.records.emplace_back(v.ID);
      }
    }
    for (const auto& [k, v] : this->index.enums.entries) {
      if (isChild(ns, v)) {
        ns.enums.emplace_back(v.ID);
      }
    }
    for (const auto& [k, v] : this->index.namespaces.entries) {
      if (isChild(ns, v)) {
        ns.namespaces.emplace_back(v.ID);
      }
    }
  }
  spdlog::info("Indexer namespace resolution complete.");
}

void hdoc::indexer::Indexer::updateRecordNames() {
  spdlog::info("Indexer updating record names with inheritance information.");
  for (auto& [k, c] : this->index.records.entries) {
    if (c.baseRecords.size() > 0) {
      uint64_t count = 0;
      c.proto += " : ";
      for (const auto& baseRecord : c.baseRecords) {
        if (count > 0) {
          c.proto += ", ";
        }

        // Print the access type that indicates which kind of inheritance was used
        switch (baseRecord.access) {
        case clang::AS_public:
          c.proto += "public ";
          break;
        case clang::AS_private:
          c.proto += "private ";
          break;
        case clang::AS_protected:
          c.proto += "protected ";
          break;
        case clang::AS_none:
        // intentional fallthrough
        default:
          break;
        }

        c.proto += baseRecord.name;
        count++;
      }
    }
  }
}

void hdoc::indexer::Indexer::printStats() const {
  // Size of databases in KiB
  const auto functionIndexSize  = this->index.functions.entries.size() * sizeof(hdoc::types::FunctionSymbol) / 1024;
  const auto recordIndexSize    = this->index.records.entries.size() * sizeof(hdoc::types::RecordSymbol) / 1024;
  const auto enumIndexSize      = this->index.enums.entries.size() * sizeof(hdoc::types::EnumMember) / 1024;
  const auto namespaceIndexSize = this->index.namespaces.entries.size() * sizeof(hdoc::types::NamespaceSymbol) / 1024;

  spdlog::info("Functions:  {} matches, {} indexed, {} KiB total size",
               this->index.functions.numMatches,
               this->index.functions.entries.size(),
               functionIndexSize);
  spdlog::info("Records:    {} matches, {} indexed, {} KiB total size",
               this->index.records.numMatches,
               this->index.records.entries.size(),
               recordIndexSize);
  spdlog::info("Enums:      {} matches, {} indexed, {} KiB total size",
               this->index.enums.numMatches,
               this->index.enums.entries.size(),
               enumIndexSize);
  spdlog::info("Namespaces: {} matches, {} indexed, {} KiB total size",
               this->index.namespaces.numMatches,
               this->index.namespaces.entries.size(),
               namespaceIndexSize);
}

void hdoc::indexer::Indexer::pruneMethods() {
  // If a method's parent isn't in the index, it was filtered out and not indexed.
  // ergo, it's children shouldn't be indexed either, so we remove them
  std::vector<hdoc::types::SymbolID> toBePruned;
  for (const auto& [k, v] : this->index.functions.entries) {
    if (v.isRecordMember && !this->index.records.contains(v.parentNamespaceID)) {
      toBePruned.emplace_back(k);
    }
  }
  // Remove methods from index
  for (const auto& deadSymbolID : toBePruned) {
    this->index.functions.entries.erase(deadSymbolID);
  }
  spdlog::info("Pruned {} functions from the database.", toBePruned.size());
}

void hdoc::indexer::Indexer::pruneTypeRefs() {
  for (auto& [k, v] : this->index.functions.entries) {
    if (this->index.records.contains(v.returnType.id) == false) {
      v.returnType.id = hdoc::types::SymbolID();
    }

    for (auto& param : v.params) {
      if (this->index.records.contains(param.type.id) == false) {
        param.type.id = hdoc::types::SymbolID();
      }
    }
  }

  for (auto& [k, v] : this->index.records.entries) {
    for (auto& var : v.vars) {
      if (this->index.records.contains(var.type.id) == false) {
        var.type.id = hdoc::types::SymbolID();
      }
    }
  }
}

const hdoc::types::Index* hdoc::indexer::Indexer::dump() const {
  return &this->index;
}
