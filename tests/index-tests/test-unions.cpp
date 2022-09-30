// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "common.hpp"

TEST_CASE("Union decl") {
  const std::string code = R"(
    union Foo {
      int a;
      bool b;
    };
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 1, 0, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.type == "union");
  CHECK(s.proto == "union Foo");
  CHECK(s.vars.size() == 2);
  CHECK(s.methodIDs.size() == 0);
  CHECK(s.baseRecords.size() == 0);

  CHECK(s.vars[0].isStatic == false);
  CHECK(s.vars[0].name == "a");
  CHECK(s.vars[0].type.name == "int");
  CHECK(s.vars[0].type.id.raw() == 0);
  CHECK(s.vars[0].defaultValue == "");
  CHECK(s.vars[0].docComment == "");
  CHECK(s.vars[0].access == clang::AS_public);

  CHECK(s.vars[1].isStatic == false);
  CHECK(s.vars[1].name == "b");
  CHECK(s.vars[1].type.name == "bool");
  CHECK(s.vars[1].type.id.raw() == 0);
  CHECK(s.vars[1].defaultValue == "");
  CHECK(s.vars[1].docComment == "");
  CHECK(s.vars[1].access == clang::AS_public);
}

TEST_CASE("Function with union as a parameter") {
  const std::string code = R"(
    union Foo {
      int a : 5;
      bool b : 3;
    };

    void act(Foo*) {}
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 1, 1, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.type == "union");
  CHECK(s.proto == "union Foo");
  CHECK(s.vars.size() == 2);
  CHECK(s.methodIDs.size() == 0);
  CHECK(s.baseRecords.size() == 0);

  CHECK(s.vars[0].isStatic == false);
  CHECK(s.vars[0].name == "a");
  CHECK(s.vars[0].type.name == "int");
  CHECK(s.vars[0].type.id.raw() == 0);
  CHECK(s.vars[0].defaultValue == "");
  CHECK(s.vars[0].docComment == "");
  CHECK(s.vars[0].access == clang::AS_public);

  CHECK(s.vars[1].isStatic == false);
  CHECK(s.vars[1].name == "b");
  CHECK(s.vars[1].type.name == "bool");
  CHECK(s.vars[1].type.id.raw() == 0);
  CHECK(s.vars[1].defaultValue == "");
  CHECK(s.vars[1].docComment == "");
  CHECK(s.vars[1].access == clang::AS_public);

  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "act");
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

  CHECK(f.proto == "void act(Foo *)");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");

  CHECK(f.params.size() == 1);
  CHECK(f.params[0].name == "");
  CHECK(f.params[0].type.name == "Foo *");
  CHECK(f.params[0].type.id == s.ID);
  CHECK(f.params[0].docComment == "");
  CHECK(f.params[0].defaultValue == "");
}

TEST_CASE("Anonymous struct in a union") {
  const std::string code = R"(
    union vector3 {
      struct { float x, y, z; };
      float v[3];
    };
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 1, 0, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "vector3");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.type == "union");
  CHECK(s.proto == "union vector3");
  CHECK(s.vars.size() == 2);
  CHECK(s.methodIDs.size() == 0);
  CHECK(s.baseRecords.size() == 0);

  CHECK(s.vars[0].isStatic == false);
  CHECK(s.vars[0].name == "");
  CHECK(s.vars[0].type.name == "anonymous struct/union");
  CHECK(s.vars[0].type.id.raw() == 0);
  CHECK(s.vars[0].defaultValue == "");
  CHECK(s.vars[0].docComment == "");
  CHECK(s.vars[0].access == clang::AS_public);

  CHECK(s.vars[1].isStatic == false);
  CHECK(s.vars[1].name == "v");
  CHECK(s.vars[1].type.name == "float[3]");
  CHECK(s.vars[1].type.id.raw() == 0);
  CHECK(s.vars[1].defaultValue == "");
  CHECK(s.vars[1].docComment == "");
  CHECK(s.vars[1].access == clang::AS_public);
}
