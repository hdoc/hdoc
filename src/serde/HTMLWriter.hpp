// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include "llvm/Support/ThreadPool.h"

#include "types/Config.hpp"
#include "types/Index.hpp"

namespace hdoc {
namespace serde {

/// @brief Serialize hdoc's index to HTML files
class HTMLWriter {
public:
  HTMLWriter(const hdoc::types::Index* index, const hdoc::types::Config* cfg, llvm::ThreadPool& pool);
  void printFunctions() const;
  void printRecords() const;
  void printRecord(const hdoc::types::RecordSymbol& c) const;
  void printNamespaces() const;
  void printEnums() const;
  void printEnum(const hdoc::types::EnumSymbol& e) const;

  /// @brief Print the search page for the documentation
  void printSearchPage() const;

  /// @brief Print the index.html page for the documentation
  void printProjectIndex() const;

  /// @brief Convert Markdown files to HTML and save them to the filesystem
  void processMarkdownFiles() const;

private:
  const hdoc::types::Index*  index;
  const hdoc::types::Config* cfg;
  llvm::ThreadPool&          pool;
};
std::string getHyperlinkedFunctionProto(const std::string_view proto, const hdoc::types::FunctionSymbol& f);
std::string clangFormat(const std::string_view s, const uint64_t& columnLimit = 50);
std::string getBareTypeName(const std::string_view typeName);
} // namespace serde
} // namespace hdoc
