// Copyright 2019-2021 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "common.hpp"

TEST_CASE("Class with constructor definition") {
  const std::string code = R"(
    class Foo {
      public:
        Foo() {}
      };

      void bar() {
        Foo  f;
        Foo* f2 = new Foo();
      };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 2, 0, 0);

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

  std::optional<hdoc::types::FunctionSymbol> o1 = findByName(index.functions, "Foo");
  std::optional<hdoc::types::FunctionSymbol> o2 = findByName(index.functions, "bar");

  CHECK(o1);
  CHECK(o2);

  hdoc::types::FunctionSymbol f1 = *o1;
  CHECK(f1.name == "Foo");
  CHECK(f1.briefComment == "");
  CHECK(f1.docComment == "");
  CHECK(f1.ID.str().size() == 16);
  CHECK(f1.parentNamespaceID == s.ID);

  hdoc::types::FunctionSymbol f2 = *o1;
  CHECK(f2.name == "Foo");
  CHECK(f2.briefComment == "");
  CHECK(f2.docComment == "");
  CHECK(f2.ID.str().size() == 16);
  CHECK(f2.parentNamespaceID == s.ID);
}

TEST_CASE("Destructor") {
  const std::string code = R"(
    class Foo {
    public:
      Foo() {}
      ~Foo(){};
    };

    void bar() {
      Foo f;
    }
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 3, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.type == "class");
  CHECK(s.proto == "class Foo");
  CHECK(s.vars.size() == 0);
  CHECK(s.methodIDs.size() == 2);
  CHECK(s.baseRecords.size() == 0);

  std::optional<hdoc::types::FunctionSymbol> o1 = findByName(index.functions, "Foo");
  std::optional<hdoc::types::FunctionSymbol> o2 = findByName(index.functions, "~Foo");
  std::optional<hdoc::types::FunctionSymbol> o3 = findByName(index.functions, "bar");

  CHECK(o1);
  CHECK(o2);
  CHECK(o3);

  hdoc::types::FunctionSymbol f1 = *o1;
  CHECK(f1.name == "Foo");
  CHECK(f1.briefComment == "");
  CHECK(f1.docComment == "");
  CHECK(f1.ID.str().size() == 16);
  CHECK(f1.parentNamespaceID == s.ID);

  CHECK(f1.isRecordMember == true);
  CHECK(f1.isConstexpr == false);
  CHECK(f1.isConsteval == false);
  CHECK(f1.isInline == false);
  CHECK(f1.isConst == false);
  CHECK(f1.isVolatile == false);
  CHECK(f1.isRestrict == false);
  CHECK(f1.isVirtual == false);
  CHECK(f1.isVariadic == false);
  CHECK(f1.isNoExcept == false);
  CHECK(f1.hasTrailingReturn == false);
  CHECK(f1.isCtorOrDtor == true);

  CHECK(f1.access == clang::AS_public);
  CHECK(f1.storageClass == clang::SC_None);
  CHECK(f1.refQualifier == clang::RQ_None);

  CHECK(f1.proto == "Foo()");
  CHECK(f1.returnType.name == "");
  CHECK(f1.returnType.id.raw() ==0);
  CHECK(f1.returnTypeDocComment == "");

  hdoc::types::FunctionSymbol f2 = *o2;
  CHECK(f2.name == "~Foo");
  CHECK(f2.briefComment == "");
  CHECK(f2.docComment == "");
  CHECK(f2.ID.str().size() == 16);
  CHECK(f2.parentNamespaceID == s.ID);

  CHECK(f2.isRecordMember == true);
  CHECK(f2.isConstexpr == false);
  CHECK(f2.isConsteval == false);
  CHECK(f2.isInline == false);
  CHECK(f2.isConst == false);
  CHECK(f2.isVolatile == false);
  CHECK(f2.isRestrict == false);
  CHECK(f2.isVirtual == false);
  CHECK(f2.isVariadic == false);
  CHECK(f2.isNoExcept == false);
  CHECK(f2.hasTrailingReturn == false);
  CHECK(f2.isCtorOrDtor == true);

  CHECK(f2.access == clang::AS_public);
  CHECK(f2.storageClass == clang::SC_None);
  CHECK(f2.refQualifier == clang::RQ_None);

  CHECK(f2.proto == "~Foo()");
  CHECK(f2.returnType.name == "");
  CHECK(f2.returnType.id.raw() ==0);
  CHECK(f2.returnTypeDocComment == "");

  hdoc::types::FunctionSymbol f3 = *o3;
  CHECK(f3.name == "bar");
  CHECK(f3.briefComment == "");
  CHECK(f3.docComment == "");
  CHECK(f3.ID.str().size() == 16);
  CHECK(f3.parentNamespaceID.raw() == 0);

  CHECK(f3.isRecordMember == false);
  CHECK(f3.isConstexpr == false);
  CHECK(f3.isConsteval == false);
  CHECK(f3.isInline == false);
  CHECK(f3.isConst == false);
  CHECK(f3.isVolatile == false);
  CHECK(f3.isRestrict == false);
  CHECK(f3.isVirtual == false);
  CHECK(f3.isVariadic == false);
  CHECK(f3.isNoExcept == false);
  CHECK(f3.hasTrailingReturn == false);
  CHECK(f3.isCtorOrDtor == false);

  CHECK(f3.access == clang::AS_none);
  CHECK(f3.storageClass == clang::SC_None);
  CHECK(f3.refQualifier == clang::RQ_None);

  CHECK(f3.proto == "void bar()");
  CHECK(f3.returnType.name == "void");
  CHECK(f3.returnType.id.raw() ==0);
  CHECK(f3.returnTypeDocComment == "");
}
