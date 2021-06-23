// Copyright 2019-2021 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include "llvm/Support/ThreadPool.h"

#include "types/Config.hpp"
#include "types/Index.hpp"

namespace hdoc::indexer {
/// @brief Index all of the code in a project into hdoc's internal representation
class Indexer {
public:
  Indexer(const hdoc::types::Config* cfg, llvm::ThreadPool& pool) : cfg(cfg), pool(pool) {}
  /// @brief Run the indexer over project code
  void run();

  /// @brief Update the declaration of the all records to indicate records they inherit
  /// from and the type of inheritance. This must be done after all records are
  /// parsed as the inherited records might not be in the database at parse-time.
  void updateRecordNames();

  /// @brief Update NamespaceSymbols with the IDs of their children
  void resolveNamespaces();

  /// @brief Remove orphaned methods from the Index
  void pruneMethods();

  /// @brief Remove TypeRefs that aren't in the Index
  /// Some TypeRefs might have SymbolIDs that aren't in the Index, for example
  /// if they're in a third-party library that isn't indexed.
  /// We need to remove them prior to HTML serialization to ensure we don't have dead links.
  void pruneTypeRefs();

  /// @brief Print the number of matches, indexed entries, and size of the database for each type.
  void printStats() const;

  /// @brief Dump the index for use in serde
  const hdoc::types::Index* dump() const;

private:
  hdoc::types::Index         index;
  const hdoc::types::Config* cfg;
  llvm::ThreadPool&          pool;
};

} // namespace hdoc::indexer
