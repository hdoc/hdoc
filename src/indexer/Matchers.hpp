#pragma once

#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

#include "types/Config.hpp"
#include "types/Index.hpp"

namespace hdoc::indexer::matchers {
class RecordMatcher : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& Result);
  RecordMatcher(hdoc::types::Index* index, const hdoc::types::Config* cfg) : index(index), cfg(cfg) {}
  clang::ast_matchers::DeclarationMatcher matcher =
      clang::ast_matchers::cxxRecordDecl(clang::ast_matchers::unless(clang::ast_matchers::anyOf(
                                             clang::ast_matchers::isImplicit(),
                                             clang::ast_matchers::isInStdNamespace(),
                                             clang::ast_matchers::isExpansionInSystemHeader(),
                                             clang::ast_matchers::isTemplateInstantiation(),
                                             clang::ast_matchers::isExplicitTemplateSpecialization())))
          .bind("record");
  hdoc::types::Index*        index;
  const hdoc::types::Config* cfg;
};

class FunctionMatcher : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& Result);
  FunctionMatcher(hdoc::types::Index* index, const hdoc::types::Config* cfg) : index(index), cfg(cfg) {}
  clang::ast_matchers::DeclarationMatcher matcher =
      clang::ast_matchers::functionDecl(clang::ast_matchers::unless(clang::ast_matchers::anyOf(
                                            clang::ast_matchers::isImplicit(),
                                            clang::ast_matchers::isInStdNamespace(),
                                            clang::ast_matchers::isExpansionInSystemHeader(),
                                            clang::ast_matchers::isTemplateInstantiation(),
                                            clang::ast_matchers::isExplicitTemplateSpecialization())))
          .bind("function");
  hdoc::types::Index*        index;
  const hdoc::types::Config* cfg;
};

class EnumMatcher : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& Result);
  EnumMatcher(hdoc::types::Index* index, const hdoc::types::Config* cfg) : index(index), cfg(cfg) {}
  clang::ast_matchers::DeclarationMatcher matcher =
      clang::ast_matchers::enumDecl(
          clang::ast_matchers::unless(clang::ast_matchers::anyOf(clang::ast_matchers::isInStdNamespace(),
                                                                 clang::ast_matchers::isExpansionInSystemHeader(),
                                                                 clang::ast_matchers::isInstantiated(),
                                                                 clang::ast_matchers::isImplicit())))
          .bind("enum");
  hdoc::types::Index*        index;
  const hdoc::types::Config* cfg;
};

class NamespaceMatcher : public clang::ast_matchers::MatchFinder::MatchCallback {
public:
  virtual void run(const clang::ast_matchers::MatchFinder::MatchResult& Result);
  NamespaceMatcher(hdoc::types::Index* index, const hdoc::types::Config* cfg) : index(index), cfg(cfg) {}
  clang::ast_matchers::DeclarationMatcher matcher =
      clang::ast_matchers::namespaceDecl(
          clang::ast_matchers::unless(clang::ast_matchers::anyOf(clang::ast_matchers::isInStdNamespace(),
                                                                 clang::ast_matchers::isExpansionInSystemHeader(),
                                                                 clang::ast_matchers::isInstantiated(),
                                                                 clang::ast_matchers::isImplicit())))
          .bind("namespace");
  hdoc::types::Index*        index;
  const hdoc::types::Config* cfg;
};
} // namespace hdoc::indexer::matchers
