// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include <atomic>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

#include "types/Symbols.hpp"

namespace hdoc::types {
/// @brief Stores values for a given type of Symbol
template <typename T> struct Database {
  std::atomic<uint32_t>                        numMatches = 0; ///< Number of matches
  std::unordered_map<hdoc::types::SymbolID, T> entries;        ///< Hashmap that stores the entries

  /// @brief Reserve a space for the given SymbolID, to be updated later
  T& reserve(const hdoc::types::SymbolID& id) {
    this->mutex.lock();
    this->entries.insert(std::make_pair(id, T()));
    this->mutex.unlock();
    return this->entries[id];
  }

  /// @brief Update the entry for a given SymbolID
  void update(const hdoc::types::SymbolID& id, const T& symbol) {
    this->mutex.lock();
    this->entries[id] = symbol;
    this->mutex.unlock();
  }

  /// @brief Check if the Database contains a key
  bool contains(const hdoc::types::SymbolID& id) const {
    this->mutex.lock();
    bool res = !(this->entries.find(id) == this->entries.end());
    this->mutex.unlock();
    return res;
  }

  /// Locks the database during operations that may cause mutations
  mutable std::mutex mutex;
};

/// @brief hdoc's index, aggregating information for all of the symbols in a codebase
struct Index {
  Database<hdoc::types::FunctionSymbol>  functions;
  Database<hdoc::types::RecordSymbol>    records;
  Database<hdoc::types::EnumSymbol>      enums;
  Database<hdoc::types::NamespaceSymbol> namespaces;
};
} // namespace hdoc::types
