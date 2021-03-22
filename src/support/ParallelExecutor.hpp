#pragma once

#include "clang/Tooling/ArgumentsAdjusters.h"
#include "clang/Tooling/Execution.h"

/// @brief A cut-down reimplementation of clang's AllTUsToolExecutor
/// Removes everything we don't need, leaving a simple mechanism that executes
/// a frontend action over all files in the compilation database
class ParallelExecutor {
public:
  /// Creates a parallel executor that will run over all files in the compilation database
  /// Args holds ArgumentAdjusters that will be applied to the parser, typically include search paths
  /// If numThreads is 0, the maximum number of threads will be spawned (llvm::hardware_concurrency())
  /// otherwise, numThreads threads will be spawned
  ParallelExecutor(const clang::tooling::CompilationDatabase&            cmpdb,
                   const std::vector<clang::tooling::ArgumentsAdjuster>& args,
                   const uint32_t                                        numThreads = 0)
      : cmpdb(cmpdb), args(args), numThreads(numThreads) {}

  void execute(std::unique_ptr<clang::tooling::FrontendActionFactory> action);

private:
  const clang::tooling::CompilationDatabase&            cmpdb;
  const std::vector<clang::tooling::ArgumentsAdjuster>& args;
  const uint32_t                                        numThreads = 0;
};
