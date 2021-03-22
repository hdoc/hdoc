#pragma once

#include "types/Config.hpp"
#include "types/Index.hpp"

namespace hdoc::indexer {
/// @brief Index all of the code in a project into hdoc's internal representation
class Indexer {
public:
  Indexer(const hdoc::types::Config* cfg) : cfg(cfg) {}
  /// @brief Run the indexer over project code
  void run();

  /// @brief Update records prototypes with the records they inherit from
  void updateRecordNames();

  /// @brief Update NamespaceSymbols with the IDs of their children
  void resolveNamespaces();

  /// @brief Remove orphaned methods from the Index
  void pruneMethods();

  /// @brief Print information about how the size of the Index
  void printStats() const;

  /// @brief Dump the index for use in serde
  const hdoc::types::Index* dump() const;

private:
  hdoc::types::Index         index;
  const hdoc::types::Config* cfg;
};

} // namespace hdoc::indexer
