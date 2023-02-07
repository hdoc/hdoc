// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include <string>

#include "clang/Tooling/Execution.h"
#include "llvm/Support/ThreadPool.h"

namespace hdoc::indexer {
/// @brief A cut-down reimplementation of clang's AllTUsToolExecutor.
/// Removes everything we don't need, leaving a simple mechanism that executes
/// a frontend action over all files in the compilation database.
class ParallelExecutor {
public:
  /// Creates a parallel executor that will run over all files in the compilation database.
  /// Args holds ArgumentAdjusters that will be applied to the parser, typically includes header search paths.
  ParallelExecutor(const clang::tooling::CompilationDatabase& cmpdb,
                   const std::vector<std::string>&            includePaths,
                   llvm::ThreadPool&                          pool,
                   const uint32_t                             debugLimitNumIndexedFiles)
      : cmpdb(cmpdb), includePaths(includePaths), pool(pool), debugLimitNumIndexedFiles(debugLimitNumIndexedFiles) {}

  void execute(std::unique_ptr<clang::tooling::FrontendActionFactory> action);

private:
  const clang::tooling::CompilationDatabase& cmpdb;
  const std::vector<std::string>&            includePaths;
  llvm::ThreadPool&                          pool;
  const uint32_t                             debugLimitNumIndexedFiles = 0;
};
} // namespace hdoc::indexer
