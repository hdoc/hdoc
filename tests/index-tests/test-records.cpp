// Copyright 2019-2021 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "common.hpp"

TEST_CASE("Class member") {
  const std::string code = R"(
    class Foo {
      Foo* member;
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 0, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.type == "class");
  CHECK(s.proto == "class Foo");
  CHECK(s.vars.size() == 1);
  CHECK(s.methodIDs.size() == 0);
  CHECK(s.baseRecords.size() == 0);

  CHECK(s.vars[0].isStatic == false);
  CHECK(s.vars[0].name == "member");
  CHECK(s.vars[0].type.name == "Foo *");
  CHECK(s.vars[0].type.id == s.ID);
  CHECK(s.vars[0].defaultValue == "");
  CHECK(s.vars[0].docComment == "");
  CHECK(s.vars[0].access == clang::AS_private);
}

TEST_CASE("Class members with default values") {
  const std::string code = R"(
    class Foo {
      Foo* a = nullptr;
      int  b = 10;
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 0, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.type == "class");
  CHECK(s.proto == "class Foo");
  CHECK(s.vars.size() == 2);
  CHECK(s.methodIDs.size() == 0);
  CHECK(s.baseRecords.size() == 0);

  CHECK(s.vars[0].isStatic == false);
  CHECK(s.vars[0].name == "a");
  CHECK(s.vars[0].type.name == "Foo *");
  CHECK(s.vars[0].type.id == s.ID);
  CHECK(s.vars[0].defaultValue == "nullptr");
  CHECK(s.vars[0].docComment == "");
  CHECK(s.vars[0].access == clang::AS_private);

  CHECK(s.vars[1].isStatic == false);
  CHECK(s.vars[1].name == "b");
  CHECK(s.vars[1].type.name == "int");
  CHECK(s.vars[1].type.id.raw() == 0);
  CHECK(s.vars[1].defaultValue == "10");
  CHECK(s.vars[1].docComment == "");
  CHECK(s.vars[1].access == clang::AS_private);
}

TEST_CASE("Class with static member") {
  const std::string code = R"(
    class Foo {
      static Foo* member;
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 0, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.type == "class");
  CHECK(s.proto == "class Foo");
  CHECK(s.vars.size() == 1);
  CHECK(s.methodIDs.size() == 0);
  CHECK(s.baseRecords.size() == 0);

  CHECK(s.vars[0].isStatic == true);
  CHECK(s.vars[0].name == "member");
  CHECK(s.vars[0].type.name == "Foo *");
  CHECK(s.vars[0].type.id == s.ID);
  CHECK(s.vars[0].defaultValue == "");
  CHECK(s.vars[0].docComment == "");
  CHECK(s.vars[0].access == clang::AS_private);
}

TEST_CASE("Incomplete record definitions") {
  // hdoc ignores forward declarations, so nothing in this
  // snippet should be indexed.
  const std::string code = R"(
    struct Foo;
    class Bar;
    union Baz;
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 0, 0, 0);
}

TEST_CASE("Class with const member function") {
  const std::string code = R"(
    class Foo {
      void foo(const int a) const {}
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 1, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.type == "class");
  CHECK(s.proto == "class Foo");
  CHECK(s.vars.size() == 0);
  CHECK(s.methodIDs.size() == 1);
  CHECK(s.baseRecords.size() == 0);

  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "foo");
  CHECK(f.briefComment == "");
  CHECK(f.docComment == "");
  CHECK(f.ID.str().size() == 16);
  CHECK(f.parentNamespaceID == s.ID);

  CHECK(f.isRecordMember == true);
  CHECK(f.isConstexpr == false);
  CHECK(f.isConsteval == false);
  CHECK(f.isInline == false);
  CHECK(f.isConst == true);
  CHECK(f.isVolatile == false);
  CHECK(f.isRestrict == false);
  CHECK(f.isVirtual == false);
  CHECK(f.isVariadic == false);
  CHECK(f.isNoExcept == false);
  CHECK(f.hasTrailingReturn == false);
  CHECK(f.isCtorOrDtor == false);

  CHECK(f.access == clang::AS_private);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_None);

  CHECK(f.proto == "void foo(const int a) const");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");

  CHECK(f.params.size() == 1);
  CHECK(f.params[0].name == "a");
  CHECK(f.params[0].type.name == "const int");
  CHECK(f.params[0].type.id.raw() == 0);
  CHECK(f.params[0].docComment == "");
  CHECK(f.params[0].defaultValue == "");
}
