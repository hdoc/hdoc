// Copyright 2019-2021 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "support/StringUtils.hpp"

#include <algorithm>

namespace hdoc::utils {
void ltrim(std::string& s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

void rtrim(std::string& s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

std::string replaceAll(std::string& str, const std::string& oldvalue, const std::string& newvalue) {
  size_t start = 0;

  // while we are not at the end of the string, find the oldvalue string
  while ((start = str.find(oldvalue, start)) != std::string::npos) {
    str.replace(start, oldvalue.length(), newvalue);
    start += newvalue.length();
  }

  return str;
}

std::size_t replaceFirst(std::string& str, const std::string& oldvalue, const std::string& newvalue, std::size_t pos) {
  std::size_t index = str.find(oldvalue, pos);
  if (index != std::string::npos) {
    str.replace(index, oldvalue.size(), newvalue);
  }
  return index + newvalue.size();
}

} // namespace hdoc::utils
