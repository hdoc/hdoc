// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchersMacros.h"

#include "types/Config.hpp"
#include "types/Index.hpp"

#include <filesystem>

namespace hdoc::indexer::matchers {

AST_MATCHER_P2(clang::Decl, shouldBeIgnored, std::vector<std::string>, ignoreList, std::filesystem::path, rootDir) {
  (void)Builder; // Avoid unused variable warning
  if (ignoreList.size() == 0) {
    return false;
  }

  auto& sourceManager = Finder->getASTContext().getSourceManager();
  auto  expansionLoc  = sourceManager.getExpansionLoc(Node.getBeginLoc());
  if (expansionLoc.isInvalid()) {
    return false;
  }

  auto fileEntry = sourceManager.getFileEntryForID(sourceManager.getFileID(expansionLoc));
  if (!fileEntry) {
    return false;
  }

  auto filename = std::filesystem::relative(std::filesystem::path(fileEntry->getName().str()), rootDir).string();
  for (const auto& substr : ignoreList) {
    if (filename.find(substr) != std::string::npos) {
      return true;
    }
  }
  return false;
} // namespace internal

class RecordMatcher : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& Result);
  RecordMatcher(hdoc::types::Index* index, const hdoc::types::Config* cfg) : index(index), cfg(cfg) {}
  hdoc::types::Index*        index;
  const hdoc::types::Config* cfg;

  clang::ast_matchers::DeclarationMatcher getMatcher() {
    return clang::ast_matchers::cxxRecordDecl(
               clang::ast_matchers::isDefinition(),
               clang::ast_matchers::unless(clang::ast_matchers::anyOf(
                   clang::ast_matchers::hasAncestor(
                       clang::ast_matchers::namespaceDecl(clang::ast_matchers::isAnonymous())),
                   clang::ast_matchers::isImplicit(),
                   clang::ast_matchers::isInStdNamespace(),
                   clang::ast_matchers::isExpansionInSystemHeader(),
                   clang::ast_matchers::isTemplateInstantiation(),
                   clang::ast_matchers::isInstantiated(),
                   clang::ast_matchers::isExplicitTemplateSpecialization(),
                   hdoc::indexer::matchers::shouldBeIgnored(this->cfg->ignorePaths, this->cfg->rootDir))))
        .bind("record");
  }
};

class FunctionMatcher : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& Result);
  FunctionMatcher(hdoc::types::Index* index, const hdoc::types::Config* cfg) : index(index), cfg(cfg) {}
  hdoc::types::Index*        index;
  const hdoc::types::Config* cfg;

  clang::ast_matchers::DeclarationMatcher getMatcher() {
    return clang::ast_matchers::functionDecl(
               clang::ast_matchers::unless(clang::ast_matchers::anyOf(
                   clang::ast_matchers::hasAncestor(
                       clang::ast_matchers::namespaceDecl(clang::ast_matchers::isAnonymous())),
                   clang::ast_matchers::isImplicit(),
                   clang::ast_matchers::isInStdNamespace(),
                   clang::ast_matchers::isExpansionInSystemHeader(),
                   clang::ast_matchers::isTemplateInstantiation(),
                   clang::ast_matchers::isInstantiated(),
                   clang::ast_matchers::isExplicitTemplateSpecialization(),
                   hdoc::indexer::matchers::shouldBeIgnored(this->cfg->ignorePaths, this->cfg->rootDir))))
        .bind("function");
  }
};

class EnumMatcher : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& Result);
  EnumMatcher(hdoc::types::Index* index, const hdoc::types::Config* cfg) : index(index), cfg(cfg) {}
  hdoc::types::Index*                     index;
  const hdoc::types::Config*              cfg;
  clang::ast_matchers::DeclarationMatcher getMatcher() {
    return clang::ast_matchers::enumDecl(
               clang::ast_matchers::isDefinition(),
               clang::ast_matchers::unless(clang::ast_matchers::anyOf(
                   clang::ast_matchers::hasAncestor(
                       clang::ast_matchers::namespaceDecl(clang::ast_matchers::isAnonymous())),

                   clang::ast_matchers::isInStdNamespace(),
                   clang::ast_matchers::isExpansionInSystemHeader(),
                   clang::ast_matchers::isImplicit(),
                   hdoc::indexer::matchers::shouldBeIgnored(this->cfg->ignorePaths, this->cfg->rootDir))))
        .bind("enum");
  }
};

class NamespaceMatcher : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& Result);
  NamespaceMatcher(hdoc::types::Index* index, const hdoc::types::Config* cfg) : index(index), cfg(cfg) {}
  hdoc::types::Index*                     index;
  const hdoc::types::Config*              cfg;
  clang::ast_matchers::DeclarationMatcher getMatcher() {
    return clang::ast_matchers::namespaceDecl(
               clang::ast_matchers::unless(clang::ast_matchers::anyOf(
                   clang::ast_matchers::hasAncestor(
                       clang::ast_matchers::namespaceDecl(clang::ast_matchers::isAnonymous())),
                   clang::ast_matchers::isInStdNamespace(),
                   clang::ast_matchers::isExpansionInSystemHeader(),
                   clang::ast_matchers::isImplicit(),
                   hdoc::indexer::matchers::shouldBeIgnored(this->cfg->ignorePaths, this->cfg->rootDir))))
        .bind("namespace");
  }
};
} // namespace hdoc::indexer::matchers
