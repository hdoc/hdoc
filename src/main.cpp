#include "llvm/Support/Signals.h"

#include "frontend/Frontend.hpp"
#include "indexer/Indexer.hpp"
#include "serde/HTMLWriter.hpp"

int main(int argc, char** argv) {
  // Print stack trace on failure
  llvm::sys::PrintStackTraceOnErrorSignal(argv[0]);

  hdoc::types::Config      cfg;
  hdoc::frontend::Frontend frontend(argc, argv, &cfg);

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

  hdoc::serde::HTMLWriter htmlWriter(index, &cfg);
  htmlWriter.printFunctions();
  htmlWriter.printRecords();
  htmlWriter.printNamespaces();
  htmlWriter.printEnums();
  htmlWriter.printSearchPage();
  htmlWriter.processMarkdownFiles();
  htmlWriter.printProjectIndex();
}
