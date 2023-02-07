// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "support/ParallelExecutor.hpp"
#include "spdlog/spdlog.h"

#include "llvm/Support/VirtualFileSystem.h"

void hdoc::indexer::ParallelExecutor::execute(std::unique_ptr<clang::tooling::FrontendActionFactory> action) {
  std::mutex mutex;

  // Add a counter to track progress
  uint32_t    i                = 0;
  std::string totalNumFiles    = std::to_string(this->cmpdb.getAllFiles().size());
  auto        incrementCounter = [&]() {
    std::unique_lock<std::mutex> lock(mutex);
    return ++i;
  };

  std::vector<std::string> allFilesInCmpdb = this->cmpdb.getAllFiles();

  if (this->debugLimitNumIndexedFiles > 0) {
    allFilesInCmpdb.resize(this->debugLimitNumIndexedFiles);
    totalNumFiles = std::to_string(this->debugLimitNumIndexedFiles);
  }

  for (const std::string& file : allFilesInCmpdb) {
    this->pool.async(
        [&](const std::string path) {
          spdlog::info("[{}/{}] processing {}", incrementCounter(), totalNumFiles, path);

          // Each thread gets an independent copy of a VFS to allow different concurrent working directories
          llvm::IntrusiveRefCntPtr<llvm::vfs::FileSystem> FS = llvm::vfs::createPhysicalFileSystem().release();
          clang::tooling::ClangTool Tool(this->cmpdb, {path}, std::make_shared<clang::PCHContainerOperations>(), FS);

          // Append argument adjusters so that system includes and others are picked up on
          // TODO: determine if the -fsyntax-only flag actually does anything
          Tool.appendArgumentsAdjuster(clang::tooling::getClangStripOutputAdjuster());
          Tool.appendArgumentsAdjuster(clang::tooling::getClangStripDependencyFileAdjuster());
          Tool.appendArgumentsAdjuster(clang::tooling::getClangSyntaxOnlyAdjuster());
          Tool.appendArgumentsAdjuster(clang::tooling::getInsertArgumentAdjuster(
              this->includePaths, clang::tooling::ArgumentInsertPosition::END));

          // Ignore all diagnostics that clang might throw. Clang often has weird diagnostic settings that don't
          // match what's in compile_commands.json, resulting in spurious errors. Instead of trying to change clang's
          // behavior, we'll ignore all diagnostics and assume that the user supplied a project that builds on their
          // machine.
          clang::IgnoringDiagConsumer ignore;
          Tool.setDiagnosticConsumer(&ignore);

          // Run the tool and print an error message if something goes wrong
          if (Tool.run(action.get())) {
            spdlog::error(
                "Clang failed to parse source file: {}. Information from this file may be missing from hdoc's output",
                path);
          }
        },
        file);
  }
  // Make sure all tasks have finished before resetting the working directory
  this->pool.wait();
}
