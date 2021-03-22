#include "support/ParallelExecutor.hpp"
#include "spdlog/spdlog.h"

#include "llvm/Support/ThreadPool.h"
#include "llvm/Support/Threading.h"
#include "llvm/Support/VirtualFileSystem.h"

void ParallelExecutor::execute(std::unique_ptr<clang::tooling::FrontendActionFactory> action) {
  std::mutex mutex;

  // Add a counter to track progress
  uint32_t          i                = 0;
  const std::string totalNumFiles    = std::to_string(this->cmpdb.getAllFiles().size());
  auto              incrementCounter = [&]() {
    std::unique_lock<std::mutex> lock(mutex);
    return ++i;
  };

  {
    llvm::ThreadPool pool(numThreads == 0 ? llvm::hardware_concurrency() : numThreads);
    for (const std::string& file : this->cmpdb.getAllFiles()) {
      pool.async(
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
            for (const auto& arg : this->args) {
              Tool.appendArgumentsAdjuster(arg);
            }

            // Run the tool and print an error message if something goes wrong
            if (Tool.run(action.get())) {
              spdlog::error("Failed to parse source file: {}", path);
            }
          },
          file);
    }
    // Make sure all tasks have finished before resetting the working directory
    pool.wait();
  }
}
