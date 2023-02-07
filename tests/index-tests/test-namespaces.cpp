// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "tests/TestUtils.hpp"

TEST_CASE("Function defined in anonymous namespace") {
  const std::string code = R"(
    namespace {
      void foo1();
    }
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 0, 0);
}

TEST_CASE("Function defined in nested namespaces with anonymous ancestor") {
  const std::string code = R"(
    namespace {
      namespace foo {
        namespace bar {
          void baz();
        }
      }
    }
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 0, 0);
}

TEST_CASE("Record and method defined in nested namespaces with anonymous ancestor") {
  const std::string code = R"(
    namespace {
      namespace foo {
        namespace bar {
          class Foo {
            void foo();
          };
        }
      }
    }
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 0, 0);
}

TEST_CASE("Record, method, and enum defined in nested namespaces with anonymous ancestor") {
  const std::string code = R"(
    namespace {
      namespace foo {
        namespace bar {
          class Baz {
            void baz();

            enum class boo {
              A,
              B,
            };
          };
        }
      }
    }
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 0, 0);
}

TEST_CASE("Record defined in anonymous namespace") {
  const std::string code = R"(
    namespace {
      class Foo {
        void foo();
      };
    }
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 0, 0);
}

TEST_CASE("Enum defined in anonymous namespace") {
  const std::string code = R"(
    namespace {
      enum class Foo {
        A,
        B,
      };

      enum Bar {
        A,
        B,
      };
    }
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 0, 0);
}

TEST_CASE("Enum defined in a record in an anonymous namespace") {
  const std::string code = R"(
    namespace {
      class Foo2 {
        enum class bar2 {
          A,
          B,
        };
        void baz2();
      };
    }
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 0, 0);
}

TEST_CASE("Function declaration in namespace") {
  const std::string code = R"(
    namespace hello {
      void foo(int a, int b);
    }
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 1, 0, 1);

  hdoc::types::NamespaceSymbol s = index.namespaces.entries.begin()->second;
  CHECK(s.name == "hello");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "foo");
  CHECK(f.briefComment == "");
  CHECK(f.docComment == "");
  CHECK(f.ID.str().size() == 16);
  CHECK(f.parentNamespaceID == s.ID);

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

  CHECK(f.proto == "void foo(int a, int b)");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.templateParams.size() == 0);

  CHECK(f.params.size() == 2);
  CHECK(f.params[0].name == "a");
  CHECK(f.params[0].type.name == "int");
  CHECK(f.params[0].type.id.raw() == 0);
  CHECK(f.params[0].docComment == "");
  CHECK(f.params[0].defaultValue == "");

  CHECK(f.params[1].name == "b");
  CHECK(f.params[1].type.name == "int");
  CHECK(f.params[1].type.id.raw() == 0);
  CHECK(f.params[1].docComment == "");
  CHECK(f.params[1].defaultValue == "");
}

TEST_CASE("Class in namespace with method declaration") {
  const std::string code = R"(
    namespace hello {
      class Foo {
        void foo();
      };
    }
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 1, 1, 0, 1);

  hdoc::types::NamespaceSymbol n = index.namespaces.entries.begin()->second;
  CHECK(n.name == "hello");
  CHECK(n.briefComment == "");
  CHECK(n.docComment == "");
  CHECK(n.ID.str().size() == 16);
  CHECK(n.parentNamespaceID.raw() == 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID == n.ID);

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
  CHECK(f.isConst == false);
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

  CHECK(f.proto == "void foo()");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
  CHECK(f.templateParams.size() == 0);
}

TEST_CASE("Class in namespace with outside method definition") {
  const std::string code = R"(
    namespace hello {
      class Foo {
        void foo();
      };

      void Foo::foo() {}
    }
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 1, 1, 0, 1);

  hdoc::types::NamespaceSymbol n = index.namespaces.entries.begin()->second;
  CHECK(n.name == "hello");
  CHECK(n.briefComment == "");
  CHECK(n.docComment == "");
  CHECK(n.ID.str().size() == 16);
  CHECK(n.parentNamespaceID.raw() == 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID == n.ID);

  CHECK(s.type == "class");
  CHECK(s.proto == "class Foo");
  CHECK(s.vars.size() == 0);
  CHECK(s.methodIDs.size() == 1);
  CHECK(s.baseRecords.size() == 0);
  CHECK(s.templateParams.size() == 0);

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
  CHECK(f.isConst == false);
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

  CHECK(f.proto == "void foo()");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
  CHECK(f.templateParams.size() == 0);
}

TEST_CASE("Class in namespace with in method definition") {
  const std::string code = R"(
    namespace hello {
      class Foo {
        void foo() {}
      };
    }
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 1, 1, 0, 1);

  hdoc::types::NamespaceSymbol n = index.namespaces.entries.begin()->second;
  CHECK(n.name == "hello");
  CHECK(n.briefComment == "");
  CHECK(n.docComment == "");
  CHECK(n.ID.str().size() == 16);
  CHECK(n.parentNamespaceID.raw() == 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID == n.ID);

  CHECK(s.type == "class");
  CHECK(s.proto == "class Foo");
  CHECK(s.vars.size() == 0);
  CHECK(s.methodIDs.size() == 1);
  CHECK(s.baseRecords.size() == 0);
  CHECK(s.templateParams.size() == 0);

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
  CHECK(f.isConst == false);
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

  CHECK(f.proto == "void foo()");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
  CHECK(f.templateParams.size() == 0);
}

TEST_CASE("Function declaration in nested namespaces") {
  const std::string code = R"(
    namespace foo {
      namespace bar {
        void baz() {}
      }
    }
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 1, 0, 2);

  std::optional<hdoc::types::NamespaceSymbol> o1 = findByName(index.namespaces, "foo");
  std::optional<hdoc::types::NamespaceSymbol> o2 = findByName(index.namespaces, "bar");

  CHECK(o1);
  CHECK(o2);

  hdoc::types::NamespaceSymbol n1 = *o1;
  CHECK(n1.name == "foo");
  CHECK(n1.briefComment == "");
  CHECK(n1.docComment == "");
  CHECK(n1.ID.str().size() == 16);
  CHECK(n1.parentNamespaceID.raw() == 0);

  hdoc::types::NamespaceSymbol n2 = *o2;
  CHECK(n2.name == "bar");
  CHECK(n2.briefComment == "");
  CHECK(n2.docComment == "");
  CHECK(n2.ID.str().size() == 16);
  CHECK(n2.parentNamespaceID == n1.ID);

  hdoc::types::FunctionSymbol f = index.functions.entries.begin()->second;
  CHECK(f.name == "baz");
  CHECK(f.briefComment == "");
  CHECK(f.docComment == "");
  CHECK(f.ID.str().size() == 16);
  CHECK(f.parentNamespaceID == n2.ID);

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

  CHECK(f.proto == "void baz()");
  CHECK(f.returnType.name == "void");
  CHECK(f.returnType.id.raw() == 0);
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
  CHECK(f.templateParams.size() == 0);
}

TEST_CASE("Namespace alias") {
  const std::string code = R"(
    namespace foo {
        namespace bar {
             namespace baz {
                 int qux = 42;
             }
        }
    }

    namespace fbz = foo::bar::baz;
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 0, 3);

  std::optional<hdoc::types::NamespaceSymbol> o1 = findByName(index.namespaces, "foo");
  std::optional<hdoc::types::NamespaceSymbol> o2 = findByName(index.namespaces, "bar");
  std::optional<hdoc::types::NamespaceSymbol> o3 = findByName(index.namespaces, "baz");

  CHECK(o1);
  CHECK(o2);
  CHECK(o3);

  hdoc::types::NamespaceSymbol n1 = *o1;
  CHECK(n1.name == "foo");
  CHECK(n1.briefComment == "");
  CHECK(n1.docComment == "");
  CHECK(n1.ID.str().size() == 16);
  CHECK(n1.parentNamespaceID.raw() == 0);

  hdoc::types::NamespaceSymbol n2 = *o2;
  CHECK(n2.name == "bar");
  CHECK(n2.briefComment == "");
  CHECK(n2.docComment == "");
  CHECK(n2.ID.str().size() == 16);
  CHECK(n2.parentNamespaceID == n1.ID);

  hdoc::types::NamespaceSymbol n3 = *o3;
  CHECK(n3.name == "baz");
  CHECK(n3.briefComment == "");
  CHECK(n3.docComment == "");
  CHECK(n3.ID.str().size() == 16);
  CHECK(n3.parentNamespaceID == n2.ID);
}
