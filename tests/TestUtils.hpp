// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include "doctest.hpp"
#include "types/Config.hpp"
#include "types/Index.hpp"

#include <optional>
#include <string>

void runOverCode(const std::string_view    code,
                 hdoc::types::Index&       index,
                 const hdoc::types::Config cfg = hdoc::types::Config());

void checkIndexSizes(const hdoc::types::Index& index,
                     const uint32_t            recordsSize,
                     const uint32_t            functionsSize,
                     const uint32_t            enumsSize,
                     const uint32_t            namespacesSize);

/// Get an element in the database by its name, used in the unit tests when we have multiple symbols.
/// This obviously doesn't work when you have multiple items in the database with the same name.
/// Don't use this for anything outside of the tests, which are a strictly-controlled environment.
template <typename T> std::optional<const T> findByName(const hdoc::types::Database<T>& db, const std::string& name) {
  for (const auto& [k, v] : db.entries) {
    if (v.name == name) {
      return v;
    }
  }
  return std::nullopt;
}
