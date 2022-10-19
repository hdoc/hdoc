// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace hdoc::types {

/// @brief Indicates the type of hdoc binary.
enum class BinaryType {
  Full,   ///< The "full" version of hdoc which is capable of saving documentation locally
          ///< This is produced by compiling hdoc locally, or using the Enterprise edition of hdoc.
  Client, ///< "hdoc-client", which is the free precompiled binary provided on hdoc.io or compiled locally.
  Server, ///< For internal hdoc usage.
};

/// @brief Stores configuration data that hdoc uses for indexing and serialization
struct Config {
  bool                     initialized       = false; ///< Is this object initialized?
  bool                     useSystemIncludes = true;  ///< Use system compiler include paths by default
  uint32_t                 numThreads        = 0; ///< Number of threads to be used during indexing (0 == all available)
  BinaryType               binaryType        = hdoc::types::BinaryType::Full; ///< What type of hdoc is this?
  std::filesystem::path    rootDir;                      ///< Path to the root of the repo directory where .hdoc.toml is
  std::filesystem::path    compileCommandsJSON;          ///< Path to compile_commands.json
  std::filesystem::path    outputDir;                    ///< Path of where documentation is saved
  std::string              projectName;                  ///< Name of the project
  std::string              projectVersion;               ///< Project version
  std::string              timestamp;                    ///< Timestamp of this run
  std::string              hdocVersion;                  ///< hdoc git commit hash
  std::string              gitRepoURL;                   ///< URL prefix of a GitHub or GitLab repo for source links
  std::string              gitDefaultBranch;             ///< default branch of the git repo
  std::vector<std::string> includePaths;                 ///< Include paths passed on to Clang
  std::vector<std::string> ignorePaths;                  ///< Paths from which matches should be ignored
  bool                     ignorePrivateMembers = false; ///< Should private members of records be ignored?
  std::filesystem::path    homepage;                     ///< Path to "homepage" markdown file
  std::vector<std::filesystem::path> mdPaths;            ///< Paths to markdown pages

  uint32_t debugLimitNumIndexedFiles; ///< Limit the number of files to index (0 == index all files)

  /// @brief Returns a string with the form "PROJECT_NAME PROJECT_VERSION documentation"
  /// if this->projectVersion has a value, otherwise returns "PROJECT_NAME documentation".
  ///
  /// projectVersion is an optional configuration parameter (to accomodate for projects that have an unversioned
  /// main branch). To avoid having many checks for `if (cfg->projectVersion == "")` in the HTML serializer,
  /// we have one function here that covers both cases and returns the appropriate string for use in the serializer.
  std::string getPageTitleSuffix() const {
    if (this->projectVersion == "") {
      return this->projectName + " documentation";
    } else {
      return this->projectName + " " + this->projectVersion + " documentation";
    }
  }
};
} // namespace hdoc::types
