// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include <vector>

#include "types/Config.hpp"
#include "types/Index.hpp"

/// Returns a vector of all SymbolIDs in a given database
/// Useful for getting a vector of SymbolIDs that will be sorted
template <typename T> static std::vector<hdoc::types::SymbolID> map2vec(const hdoc::types::Database<T>& db) {
  std::vector<hdoc::types::SymbolID> IDs;
  IDs.reserve(db.entries.size());
  for (const auto& [k, v] : db.entries) {
    IDs.emplace_back(k);
  }
  return IDs;
}

/// Sort a vector of SymbolIDs alphabetically by the name of the Symbol they point to
/// Note: all members of IDs need to be of type T
template <typename T>
static std::vector<hdoc::types::SymbolID> getSortedIDs(const std::vector<hdoc::types::SymbolID>& IDs,
                                                       const hdoc::types::Database<T>&           db) {
  std::vector<T> symbols = {};
  symbols.reserve(IDs.size());
  for (const auto& id : IDs) {
    if (db.contains(id) == false) {
      continue;
    }
    symbols.emplace_back(db.entries.at(id));
  }
  std::sort(symbols.begin(), symbols.end());
  std::vector<hdoc::types::SymbolID> sortedIDs;
  sortedIDs.reserve(IDs.size());
  for (const auto& s : symbols) {
    sortedIDs.emplace_back(s.ID);
  }
  return sortedIDs;
}

/// Read the file at `path` into the string `str`.
void slurpFile(const std::filesystem::path& path, std::string& str);

/// Dump hdoc's data structures to the current working directory into the file "hdoc-payload.json".
bool dumpJSONPayload(const std::string_view data);
