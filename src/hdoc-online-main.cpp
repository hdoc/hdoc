// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "llvm/Support/Signals.h"
#include "llvm/Support/ThreadPool.h"
#include "llvm/Support/Threading.h"

#include "frontend/Frontend.hpp"
#include "indexer/Indexer.hpp"
#include "serde/SerdeUtils.hpp"
#include "serde/Serialization.hpp"

int main(int argc, char** argv) {
  // Print stack trace on failure
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);

  hdoc::types::Config cfg;
  cfg.binaryType = hdoc::types::BinaryType::Online;
  hdoc::frontend::Frontend frontend(argc, argv, &cfg);

  // Check if user is verified prior to indexing everything
  if (hdoc::serde::verify() == false) {
    return EXIT_FAILURE;
  }

  // Ensure that cfg was properly initialized
  if (!cfg.initialized) {
    return EXIT_FAILURE;
  }

  llvm::ThreadPool       pool(llvm::hardware_concurrency(cfg.numThreads));
  hdoc::indexer::Indexer indexer(&cfg, pool);
  indexer.run();
  indexer.pruneMethods();
  indexer.pruneTypeRefs();
  indexer.resolveNamespaces();
  indexer.updateRecordNames();
  indexer.printStats();
  const hdoc::types::Index* index = indexer.dump();

  const std::string data = hdoc::serde::serializeToJSON(*index, cfg);
  hdoc::serde::uploadDocs(data);

  // Ensure that cfg was properly initialized
  if (cfg.debugDumpJSONPayload) {
    bool res = dumpJSONPayload(data);
    if (res == false) {
      return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}
