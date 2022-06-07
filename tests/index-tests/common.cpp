// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "common.hpp"

#include "doctest.hpp"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/Tooling.h"

#include "indexer/Matchers.hpp"
#include "types/Symbols.hpp"

hdoc::types::Index runOverCode(const std::string& code, const hdoc::types::Config cfg) {
  hdoc::types::Index                        index;
  clang::ast_matchers::MatchFinder          Finder;
  hdoc::indexer::matchers::FunctionMatcher  FunctionFinder(&index, &cfg);
  hdoc::indexer::matchers::RecordMatcher    RecordFinder(&index, &cfg);
  hdoc::indexer::matchers::EnumMatcher      EnumFinder(&index, &cfg);
  hdoc::indexer::matchers::NamespaceMatcher NamespaceFinder(&index, &cfg);
  Finder.addMatcher(FunctionFinder.matcher, &FunctionFinder);
  Finder.addMatcher(RecordFinder.matcher, &RecordFinder);
  Finder.addMatcher(EnumFinder.matcher, &EnumFinder);
  Finder.addMatcher(NamespaceFinder.matcher, &NamespaceFinder);
  std::unique_ptr<clang::tooling::FrontendActionFactory> Factory(clang::tooling::newFrontendActionFactory(&Finder));
  clang::tooling::runToolOnCode(Factory->create(), code);
  return index;
}

void checkIndexSizes(const hdoc::types::Index& index,
                     const uint32_t            recordsSize,
                     const uint32_t            functionsSize,
                     const uint32_t            enumsSize,
                     const uint32_t            namespacesSize) {
  CHECK(index.records.entries.size() == recordsSize);
  CHECK(index.functions.entries.size() == functionsSize);
  CHECK(index.enums.entries.size() == enumsSize);
  CHECK(index.namespaces.entries.size() == namespacesSize);
}
