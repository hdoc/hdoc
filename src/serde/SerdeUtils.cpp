// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "SerdeUtils.hpp"

#include "spdlog/spdlog.h"

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>

void slurpFile(const std::filesystem::path& path, std::string& str) {
  std::ifstream t(path);

  // Reserve space in the string to avoid reallocations during slurping
  t.seekg(0, std::ios::end);
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
}

bool dumpJSONPayload(const std::string_view data) {
  std::ofstream out("hdoc-payload.json");
  if (!out) {
    spdlog::error("Failed to open hdoc-payload.json file in current working directory.");
    return false;
  }

  out << data;
  spdlog::info("hdoc-payload.json successfully written to current working directory.");
  return true;
}
