// Copyright 2019-2021 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include <string>

namespace hdoc::utils {
/// Trim any leading spaces in str.
void ltrim(std::string& s);

/// Trim any trailing spaces in str.
void rtrim(std::string& s);

/// Replace all instances of oldvalue in str with newvalue.
std::string replaceAll(std::string& str, const std::string& oldvalue, const std::string& newvalue);

///  Replace the first instance of oldvalue in str with newvalue, returning the index of the last changed character.
/// Optionally start the search after pos.
std::size_t
replaceFirst(std::string& str, const std::string& oldvalue, const std::string& newvalue, std::size_t pos = 0);
} // namespace hdoc::utils
