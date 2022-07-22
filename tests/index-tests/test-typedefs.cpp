// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "common.hpp"

TEST_CASE("Typedefed function") {
  const std::string code = R"(
    typedef int (func)(const int *a, const int *b);
    func g;
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "g");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.isRecordMember == false);
  CHECK(s.isConstexpr == false);
  CHECK(s.isConsteval == false);
  CHECK(s.isInline == false);
  CHECK(s.isConst == false);
  CHECK(s.isVolatile == false);
  CHECK(s.isRestrict == false);
  CHECK(s.isVirtual == false);
  CHECK(s.isVariadic == false);
  CHECK(s.isNoExcept == false);
  CHECK(s.hasTrailingReturn == false);
  CHECK(s.isCtorOrDtor == false);

  CHECK(s.access == clang::AS_none);
  CHECK(s.storageClass == clang::SC_None);
  CHECK(s.refQualifier == clang::RQ_None);

  // Typedef'ed functions don't inherit parameter names, only types
  CHECK(s.proto == "int g(const int *, const int *)");
  CHECK(s.returnType.name == "int");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == "");
  CHECK(s.templateParams.size() == 0);

  CHECK(s.params.size() == 2);
  CHECK(s.params[0].name == "");
  CHECK(s.params[0].type.name == "const int *");
  CHECK(s.params[0].type.id.raw() == 0);
  CHECK(s.params[0].docComment == "");
  CHECK(s.params[0].defaultValue == "");

  CHECK(s.params[1].name == "");
  CHECK(s.params[1].type.name == "const int *");
  CHECK(s.params[1].type.id.raw() == 0);
  CHECK(s.params[1].docComment == "");
  CHECK(s.params[1].defaultValue == "");
}

TEST_CASE("Type instance with 'using'") {
  const std::string code = R"(
    struct S {};
    using F = S;

    void Foo(F* param) {}
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 1, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "S");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.type == "struct");
  CHECK(s.proto == "struct S");
  CHECK(s.vars.size() == 0);
  CHECK(s.methodIDs.size() == 0);
  CHECK(s.baseRecords.size() == 0);
  CHECK(s.templateParams.size() == 0);

  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "Foo");
  CHECK(f.briefComment == "");
  CHECK(f.docComment == "");
  CHECK(f.ID.str().size() == 16);
  CHECK(f.parentNamespaceID.raw() == 0);

  CHECK(f.isRecordMember == false);
  CHECK(f.isConstexpr == false);
  CHECK(f.isConsteval == false);
  CHECK(f.isInline == false);
  CHECK(f.isConst == false);
  CHECK(f.isVolatile == false);
  CHECK(f.isRestrict == false);
  CHECK(f.isVirtual == false);
  CHECK(f.isVariadic == false);
  CHECK(f.isNoExcept == false);
  CHECK(f.hasTrailingReturn == false);
  CHECK(f.isCtorOrDtor == false);

  CHECK(f.access == clang::AS_none);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_None);

  CHECK(f.proto == "void Foo(F * param)");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.templateParams.size() == 0);

  CHECK(f.params.size() == 1);
  CHECK(f.params[0].name == "param");
  CHECK(f.params[0].type.name == "F *");
  CHECK(f.params[0].type.id == s.ID);
  CHECK(f.params[0].docComment == "");
  CHECK(f.params[0].defaultValue == "");
}
