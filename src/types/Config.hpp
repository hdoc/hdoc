// Copyright 2019-2021 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace hdoc::types {
/// @brief Stores configuration data that hdoc uses for indexing and serialization
struct Config {
  bool                     initialized       = false; ///< Is this object initialized?
  bool                     useSystemIncludes = true;  ///< Use system compiler include paths by default
  uint32_t                 numThreads        = 0; ///< Number of threads to be used during indexing (0 == all available)
  std::filesystem::path    rootDir;               ///< Path to the root of the repo directory where .hdoc.toml is
  std::filesystem::path    compileCommandsJSON;   ///< Path to compile_commands.json
  std::filesystem::path    outputDir;             ///< Path of where documentation is saved
  std::string              projectName;           ///< Name of the project
  std::string              projectVersion;        ///< Project version
  std::string              timestamp;             ///< Timestamp of this run
  std::string              hdocVersion;           ///< hdoc git commit hash
  std::string              gitRepoURL;            ///< URL prefix of a GitHub or GitLab repo for source links
  std::vector<std::string> includePaths;          ///< Include paths passed on to Clang
  std::vector<std::string> ignorePaths;           ///< Paths from which matches should be ignored
  std::filesystem::path    homepage;              ///< Path to "homepage" markdown file
  std::vector<std::filesystem::path> mdPaths;     ///< Paths to markdown pages
};
} // namespace hdoc::types
