#include "llvm/Support/Signals.h"

#include "frontend/Frontend.hpp"
#include "indexer/Indexer.hpp"
#include "serde/Serialization.hpp"

int main(int argc, char** argv) {
  // Print stack trace on failure
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);

  hdoc::types::Config      cfg;
  hdoc::frontend::Frontend frontend(argc, argv, &cfg);

  // Check if user is verified prior to indexing everything
  if (hdoc::serde::verify() == false) {
    return EXIT_FAILURE;
  }

  // Ensure that cfg was properly initialized
  if (!cfg.initialized) {
    return EXIT_FAILURE;
  }

  hdoc::indexer::Indexer indexer(&cfg);
  indexer.run();
  indexer.pruneMethods();
  indexer.resolveNamespaces();
  indexer.updateRecordNames();
  indexer.printStats();
  const hdoc::types::Index* index = indexer.dump();

  const std::string data = hdoc::serde::serialize(*index, cfg);
  hdoc::serde::uploadDocs(data);
}
