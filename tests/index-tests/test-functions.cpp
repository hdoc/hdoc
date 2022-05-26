// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "common.hpp"

TEST_CASE("Function with struct as a parameter") {
  const std::string code = R"(
    struct Foo;
    void foo(Foo* p0, Foo* p1) {}
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "foo");
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

  CHECK(s.proto == "void foo(Foo * p0, Foo * p1)");
  CHECK(s.returnType.name == "void");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == "");

  CHECK(s.params.size() == 2);
  CHECK(s.params[0].name == "p0");
  CHECK(s.params[0].type.name == "Foo *");
  // CHECK(s.params[0].type.id.raw() == 0); // ID would be 0 after pruning
  CHECK(s.params[0].docComment == "");
  CHECK(s.params[0].defaultValue == "");

  CHECK(s.params[1].name == "p1");
  CHECK(s.params[1].type.name == "Foo *");
  // CHECK(s.params[1].type.id.raw() == 0); // ID would be 0 after pruning
  CHECK(s.params[1].docComment == "");
  CHECK(s.params[1].defaultValue == "");
}

TEST_CASE("Function with unnamed parameters") {
  const std::string code = R"(
    void foo(int, int) {}
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "foo");
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

  CHECK(s.proto == "void foo(int, int)");
  CHECK(s.returnType.name == "void");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == "");

  CHECK(s.params.size() == 2);
  CHECK(s.params[0].name == "");
  CHECK(s.params[0].type.name == "int");
  CHECK(s.params[0].type.id.raw() == 0);
  CHECK(s.params[0].docComment == "");
  CHECK(s.params[0].defaultValue == "");

  CHECK(s.params[1].name == "");
  CHECK(s.params[1].type.name == "int");
  CHECK(s.params[1].type.id.raw() == 0);
  CHECK(s.params[1].docComment == "");
  CHECK(s.params[1].defaultValue == "");
}

TEST_CASE("Function default values for parameters") {
  const std::string code = R"(
    void foo(int a = 0, int b = 100) {}
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "foo");
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

  CHECK(s.proto == "void foo(int a = 0, int b = 100)");
  CHECK(s.returnType.name == "void");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == "");

  CHECK(s.params.size() == 2);
  CHECK(s.params[0].name == "a");
  CHECK(s.params[0].type.name == "int");
  CHECK(s.params[0].type.id.raw() == 0);
  CHECK(s.params[0].docComment == "");
  CHECK(s.params[0].defaultValue == "0");

  CHECK(s.params[1].name == "b");
  CHECK(s.params[1].type.name == "int");
  CHECK(s.params[1].type.id.raw() == 0);
  CHECK(s.params[1].docComment == "");
  CHECK(s.params[1].defaultValue == "100");
}

TEST_CASE("Function with trailing return type syntax") {
  const std::string code = R"(
    auto foo(int x, int y) -> int;
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "foo");
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
  CHECK(s.hasTrailingReturn == true);
  CHECK(s.isCtorOrDtor == false);

  CHECK(s.access == clang::AS_none);
  CHECK(s.storageClass == clang::SC_None);
  CHECK(s.refQualifier == clang::RQ_None);

  CHECK(s.proto == "auto foo(int x, int y) -> int");
  CHECK(s.returnType.name == "int");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == "");

  CHECK(s.params.size() == 2);
  CHECK(s.params[0].name == "x");
  CHECK(s.params[0].type.name == "int");
  CHECK(s.params[0].type.id.raw() == 0);
  CHECK(s.params[0].docComment == "");
  CHECK(s.params[0].defaultValue == "");

  CHECK(s.params[1].name == "y");
  CHECK(s.params[1].type.name == "int");
  CHECK(s.params[1].type.id.raw() == 0);
  CHECK(s.params[1].docComment == "");
  CHECK(s.params[1].defaultValue == "");
}

TEST_CASE("Function with constexpr") {
  const std::string code = R"(
    constexpr int gcd(int a, int b){
      return (b == 0) ? a : gcd(b, a % b);
    }
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "gcd");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.isRecordMember == false);
  CHECK(s.isConstexpr == true);
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

  CHECK(s.proto == "constexpr int gcd(int a, int b)");
  CHECK(s.returnType.name == "int");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == "");

  CHECK(s.params.size() == 2);
  CHECK(s.params[0].name == "a");
  CHECK(s.params[0].type.name == "int");
  CHECK(s.params[0].type.id.raw() == 0);
  CHECK(s.params[0].docComment == "");
  CHECK(s.params[0].defaultValue == "");

  CHECK(s.params[1].name == "b");
  CHECK(s.params[1].type.name == "int");
  CHECK(s.params[1].type.id.raw() == 0);
  CHECK(s.params[1].docComment == "");
  CHECK(s.params[1].defaultValue == "");
}

TEST_CASE("Member function marked volatile") {
  const std::string code = R"(
    struct Foo {
       void bar() volatile;
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 1, 0, 0);

  hdoc::types::RecordSymbol   s = index.records.entries.begin()->second;
  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "bar");
  CHECK(f.briefComment == "");
  CHECK(f.docComment == "");
  CHECK(f.ID.str().size() == 16);
  CHECK(f.parentNamespaceID == s.ID);

  CHECK(f.isRecordMember == true);
  CHECK(f.isConstexpr == false);
  CHECK(f.isConsteval == false);
  CHECK(f.isInline == false);
  CHECK(f.isConst == false);
  CHECK(f.isVolatile == true);
  CHECK(f.isRestrict == false);
  CHECK(f.isVirtual == false);
  CHECK(f.isVariadic == false);
  CHECK(f.isNoExcept == false);
  CHECK(f.hasTrailingReturn == false);
  CHECK(f.isCtorOrDtor == false);

  CHECK(f.access == clang::AS_public);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_None);

  CHECK(f.proto == "void bar() volatile");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
}

TEST_CASE("Member function with lvalue ref qualifier") {
  const std::string code = R"(
    struct Foo {
      void get() &;
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 1, 0, 0);

  hdoc::types::RecordSymbol   s = index.records.entries.begin()->second;
  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "get");
  CHECK(f.briefComment == "");
  CHECK(f.docComment == "");
  CHECK(f.ID.str().size() == 16);
  CHECK(f.parentNamespaceID == s.ID);

  CHECK(f.isRecordMember == true);
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

  CHECK(f.access == clang::AS_public);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_LValue);

  CHECK(f.proto == "void get() &");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
}

TEST_CASE("Member function with rvalue ref qualifier") {
  const std::string code = R"(
    struct Foo {
      void get() &&;
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 1, 0, 0);

  hdoc::types::RecordSymbol   s = index.records.entries.begin()->second;
  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "get");
  CHECK(f.briefComment == "");
  CHECK(f.docComment == "");
  CHECK(f.ID.str().size() == 16);
  CHECK(f.parentNamespaceID == s.ID);

  CHECK(f.isRecordMember == true);
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

  CHECK(f.access == clang::AS_public);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_RValue);

  CHECK(f.proto == "void get() &&");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
}

TEST_CASE("Member function with const lvalue ref qualifier") {
  const std::string code = R"(
    struct Foo {
      void get() const &;
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 1, 0, 0);

  hdoc::types::RecordSymbol   s = index.records.entries.begin()->second;
  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "get");
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

  CHECK(f.access == clang::AS_public);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_LValue);

  CHECK(f.proto == "void get() const &");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
}

TEST_CASE("Member function with const rvalue ref qualifier") {
  const std::string code = R"(
    struct Foo {
      void get() const &&;
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 1, 0, 0);

  hdoc::types::RecordSymbol   s = index.records.entries.begin()->second;
  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "get");
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

  CHECK(f.access == clang::AS_public);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_RValue);

  CHECK(f.proto == "void get() const &&");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
}

TEST_CASE("Noexcept function 1") {
  const std::string code = R"(
    void foo() noexcept;
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "foo");
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
  CHECK(f.isNoExcept == true);
  CHECK(f.hasTrailingReturn == false);
  CHECK(f.isCtorOrDtor == false);

  CHECK(f.access == clang::AS_none);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_None);

  CHECK(f.proto == "void foo() noexcept");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
}

TEST_CASE("Noexcept function 2") {
  const std::string code = R"(
    void foo() noexcept(true);
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "foo");
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
  CHECK(f.isNoExcept == true);
  CHECK(f.hasTrailingReturn == false);
  CHECK(f.isCtorOrDtor == false);

  CHECK(f.access == clang::AS_none);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_None);

  CHECK(f.proto == "void foo() noexcept");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
}

TEST_CASE("Variadic function") {
  const std::string code = R"(
    void simple_printf(const char* fmt...);
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "simple_printf");
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
  CHECK(f.isVariadic == true);
  CHECK(f.isNoExcept == false);
  CHECK(f.hasTrailingReturn == false);
  CHECK(f.isCtorOrDtor == false);

  CHECK(f.access == clang::AS_none);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_None);

  CHECK(f.proto == "void simple_printf(const char * fmt, ...)");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 1);

  CHECK(f.params[0].name == "fmt");
  CHECK(f.params[0].type.name == "const char *");
  CHECK(f.params[0].type.id.raw() == 0);
  CHECK(f.params[0].docComment == "");
  CHECK(f.params[0].defaultValue == "");
}

TEST_CASE("Inline function") {
  const std::string code = R"(
    inline int cube(int s) {
        return s*s*s;
    }
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "cube");
  CHECK(f.briefComment == "");
  CHECK(f.docComment == "");
  CHECK(f.ID.str().size() == 16);
  CHECK(f.parentNamespaceID.raw() == 0);

  CHECK(f.isRecordMember == false);
  CHECK(f.isConstexpr == false);
  CHECK(f.isConsteval == false);
  CHECK(f.isInline == true);
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

  CHECK(f.proto == "inline int cube(int s)");
  CHECK(f.returnType.name == "int");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 1);

  CHECK(f.params[0].name == "s");
  CHECK(f.params[0].type.name == "int");
  CHECK(f.params[0].type.id.raw() == 0);
  CHECK(f.params[0].docComment == "");
  CHECK(f.params[0].defaultValue == "");
}

TEST_CASE("Member function with trailing return type, noexcept, and rvalue reference") {
  const std::string code = R"(
    class Type {
    public:
      constexpr auto take() && noexcept -> Type;
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 1, 0, 0);

  hdoc::types::RecordSymbol c = index.records.entries.begin()->second;
  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "take");
  CHECK(f.briefComment == "");
  CHECK(f.docComment == "");
  CHECK(f.ID.str().size() == 16);
  CHECK(f.parentNamespaceID == c.ID);

  CHECK(f.isRecordMember == true);
  CHECK(f.isConstexpr == true);
  CHECK(f.isConsteval == false);
  CHECK(f.isInline == false);
  CHECK(f.isConst == false);
  CHECK(f.isVolatile == false);
  CHECK(f.isRestrict == false);
  CHECK(f.isVirtual == false);
  CHECK(f.isVariadic == false);
  CHECK(f.isNoExcept == true);
  CHECK(f.hasTrailingReturn == true);
  CHECK(f.isCtorOrDtor == false);

  CHECK(f.access == clang::AS_public);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_RValue);

  CHECK(f.proto == "constexpr auto take() && noexcept -> Type");
  CHECK(f.returnType.name == "Type");
  CHECK(f.returnType.id == c.ID);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
}

TEST_CASE("Const member function with trailing return type, noexcept, and const lvalue reference") {
  const std::string code = R"(
    class Type {
    public:
      constexpr auto borrow() const& noexcept -> const Type&;
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 1, 0, 0);

  hdoc::types::RecordSymbol c = index.records.entries.begin()->second;
  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "borrow");
  CHECK(f.briefComment == "");
  CHECK(f.docComment == "");
  CHECK(f.ID.str().size() == 16);
  CHECK(f.parentNamespaceID == c.ID);

  CHECK(f.isRecordMember == true);
  CHECK(f.isConstexpr == true);
  CHECK(f.isConsteval == false);
  CHECK(f.isInline == false);
  CHECK(f.isConst == true);
  CHECK(f.isVolatile == false);
  CHECK(f.isRestrict == false);
  CHECK(f.isVirtual == false);
  CHECK(f.isVariadic == false);
  CHECK(f.isNoExcept == true);
  CHECK(f.hasTrailingReturn == true);
  CHECK(f.isCtorOrDtor == false);

  CHECK(f.access == clang::AS_public);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_LValue);

  CHECK(f.proto == "constexpr auto borrow() const & noexcept -> const Type &");
  CHECK(f.returnType.name == "const Type &");
  CHECK(f.returnType.id == c.ID);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
}


// TODO: fix this once we're on LLVM/Clang 12
// on clang 9 the function is marked constexpr and not consteval
// TEST_CASE("Consteval function") {
//   const std::string code = R"(
//     consteval int sqr(int n) {
//       return n*n;
//     }
//   )";

//   const hdoc::types::Index index = runOverCode(code);
//   checkIndexSizes(index, 0, 1, 0, 0);

//   hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
//   CHECK(f.name == "sqr");
//   CHECK(f.briefComment == "");
//   CHECK(f.docComment == "");
//   CHECK(f.ID.str().size() == 16);
//   CHECK(f.parentNamespaceID.raw() == 0);

//   CHECK(f.isRecordMember == false);
//   CHECK(f.isConstexpr == false);
//   CHECK(f.isConsteval == true);
//   CHECK(f.isInline == false);
//   CHECK(f.isConst == true);
//   CHECK(f.isVolatile == false);
//   CHECK(f.isRestrict == false);
//   CHECK(f.isVirtual == false);
//   CHECK(f.isVariadic == false);
//   CHECK(f.isNoExcept == false);
//   CHECK(f.hasTrailingReturn == false);
//   CHECK(f.isCtorOrDtor == false);

//   CHECK(f.access == clang::AS_none);
//   CHECK(f.storageClass == clang::SC_None);
//   CHECK(f.refQualifier == clang::RQ_None);

//   CHECK(f.proto == "consteval int sqr(int n)");
//   CHECK(f.returnType == "void");
//   CHECK(f.returnTypeDocComment == "");
//   CHECK(f.params.size() == 1);

//   CHECK(f.params[0].name == "n");
//   CHECK(f.params[0].type == "int");
//   CHECK(f.params[0].docComment == "");
//   CHECK(f.params[0].defaultValue == "");
// }
