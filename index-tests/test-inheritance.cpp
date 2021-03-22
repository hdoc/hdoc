#include "common.hpp"

TEST_CASE("Class inherit") {
  const std::string code = R"(
    class Parent {};
    class Derived : public Parent {};
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 2, 0, 0, 0);

  std::optional<hdoc::types::RecordSymbol> o1 = findByName(index.records, "Parent");
  std::optional<hdoc::types::RecordSymbol> o2 = findByName(index.records, "Derived");

  CHECK(o1);
  CHECK(o2);

  hdoc::types::RecordSymbol s1 = *o1;
  CHECK(s1.name == "Parent");
  CHECK(s1.briefComment == "");
  CHECK(s1.docComment == "");
  CHECK(s1.ID.str().size() == 16);
  CHECK(s1.parentNamespaceID.raw() == 0);

  CHECK(s1.type == "class");
  CHECK(s1.proto == "class Parent");
  CHECK(s1.vars.size() == 0);
  CHECK(s1.methodIDs.size() == 0);
  CHECK(s1.baseRecords.size() == 0);

  hdoc::types::RecordSymbol s2 = *o2;
  CHECK(s2.name == "Derived");
  CHECK(s2.briefComment == "");
  CHECK(s2.docComment == "");
  CHECK(s2.ID.str().size() == 16);
  CHECK(s2.parentNamespaceID.raw() == 0);

  CHECK(s2.type == "class");
  CHECK(s2.proto == "class Derived");
  CHECK(s2.vars.size() == 0);
  CHECK(s2.methodIDs.size() == 0);
  CHECK(s2.baseRecords.size() == 1);

  CHECK(s2.baseRecords[0].id == s1.ID);
  CHECK(s2.baseRecords[0].access == clang::AS_public);
  CHECK(s2.baseRecords[0].name == "Parent");
}

TEST_CASE("Class multiple inherit") {
  const std::string code = R"(
    class Root {};
    class MiddleA : public Root {};
    class MiddleB : public Root {};
    class Derived : public MiddleA, public MiddleB {};
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 4, 0, 0, 0);

  std::optional<hdoc::types::RecordSymbol> o1 = findByName(index.records, "Root");
  std::optional<hdoc::types::RecordSymbol> o2 = findByName(index.records, "MiddleA");
  std::optional<hdoc::types::RecordSymbol> o3 = findByName(index.records, "MiddleB");
  std::optional<hdoc::types::RecordSymbol> o4 = findByName(index.records, "Derived");

  CHECK(o1);
  CHECK(o2);
  CHECK(o3);
  CHECK(o4);

  hdoc::types::RecordSymbol s1 = *o1;
  hdoc::types::RecordSymbol s2 = *o2;
  hdoc::types::RecordSymbol s3 = *o3;
  hdoc::types::RecordSymbol s4 = *o4;

  CHECK(s1.name == "Root");
  CHECK(s1.briefComment == "");
  CHECK(s1.docComment == "");
  CHECK(s1.ID.str().size() == 16);
  CHECK(s1.parentNamespaceID.raw() == 0);

  CHECK(s1.type == "class");
  CHECK(s1.proto == "class Root");
  CHECK(s1.vars.size() == 0);
  CHECK(s1.methodIDs.size() == 0);
  CHECK(s1.baseRecords.size() == 0);

  CHECK(s2.name == "MiddleA");
  CHECK(s2.briefComment == "");
  CHECK(s2.docComment == "");
  CHECK(s2.ID.str().size() == 16);
  CHECK(s2.parentNamespaceID.raw() == 0);

  CHECK(s2.type == "class");
  CHECK(s2.proto == "class MiddleA");
  CHECK(s2.vars.size() == 0);
  CHECK(s2.methodIDs.size() == 0);
  CHECK(s2.baseRecords.size() == 1);

  CHECK(s2.baseRecords[0].id == s1.ID);
  CHECK(s2.baseRecords[0].access == clang::AS_public);
  CHECK(s2.baseRecords[0].name == "Root");

  CHECK(s3.name == "MiddleB");
  CHECK(s3.briefComment == "");
  CHECK(s3.docComment == "");
  CHECK(s3.ID.str().size() == 16);
  CHECK(s3.parentNamespaceID.raw() == 0);

  CHECK(s3.type == "class");
  CHECK(s3.proto == "class MiddleB");
  CHECK(s3.vars.size() == 0);
  CHECK(s3.methodIDs.size() == 0);
  CHECK(s3.baseRecords.size() == 1);

  CHECK(s3.baseRecords[0].id == s1.ID);
  CHECK(s3.baseRecords[0].access == clang::AS_public);
  CHECK(s3.baseRecords[0].name == "Root");

  CHECK(s4.name == "Derived");
  CHECK(s4.briefComment == "");
  CHECK(s4.docComment == "");
  CHECK(s4.ID.str().size() == 16);
  CHECK(s4.parentNamespaceID.raw() == 0);

  CHECK(s4.type == "class");
  CHECK(s4.proto == "class Derived");
  CHECK(s4.vars.size() == 0);
  CHECK(s4.methodIDs.size() == 0);
  CHECK(s4.baseRecords.size() == 2);

  CHECK(s4.baseRecords[0].id == s2.ID);
  CHECK(s4.baseRecords[0].access == clang::AS_public);
  CHECK(s4.baseRecords[0].name == "MiddleA");

  CHECK(s4.baseRecords[1].id == s3.ID);
  CHECK(s4.baseRecords[1].access == clang::AS_public);
  CHECK(s4.baseRecords[1].name == "MiddleB");
}

TEST_CASE("Function override") {
  const std::string code = R"(
    class Root {
      virtual void foo();
    };
    class Derived : public Root {
      void foo() override {}
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 2, 2, 0, 0);

  std::optional<hdoc::types::RecordSymbol> a1 = findByName(index.records, "Root");
  std::optional<hdoc::types::RecordSymbol> a2 = findByName(index.records, "Derived");

  CHECK(a1);
  CHECK(a2);

  hdoc::types::RecordSymbol s1 = *a1;
  hdoc::types::RecordSymbol s2 = *a2;

  CHECK(s1.name == "Root");
  CHECK(s1.briefComment == "");
  CHECK(s1.docComment == "");
  CHECK(s1.ID.str().size() == 16);
  CHECK(s1.parentNamespaceID.raw() == 0);

  CHECK(s1.type == "class");
  CHECK(s1.proto == "class Root");
  CHECK(s1.vars.size() == 0);
  CHECK(s1.methodIDs.size() == 1);
  CHECK(s1.baseRecords.size() == 0);

  hdoc::types::FunctionSymbol f1 = index.functions.entries.at(s1.methodIDs[0]);
  CHECK(f1.name == "foo");
  CHECK(f1.briefComment == "");
  CHECK(f1.docComment == "");
  CHECK(f1.ID.str().size() == 16);
  CHECK(f1.parentNamespaceID == s1.ID);

  CHECK(f1.isRecordMember == true);
  CHECK(f1.isConstexpr == false);
  CHECK(f1.isConsteval == false);
  CHECK(f1.isInline == false);
  CHECK(f1.isConst == false);
  CHECK(f1.isVolatile == false);
  CHECK(f1.isRestrict == false);
  CHECK(f1.isVirtual == true);
  CHECK(f1.isVariadic == false);
  CHECK(f1.isNoExcept == false);
  CHECK(f1.hasTrailingReturn == false);
  CHECK(f1.isCtorOrDtor == false);

  CHECK(f1.access == clang::AS_private);
  CHECK(f1.storageClass == clang::SC_None);
  CHECK(f1.refQualifier == clang::RQ_None);

  CHECK(f1.proto == "virtual void foo()");
  CHECK(f1.returnType == "void");
  CHECK(f1.returnTypeDocComment == "");
  CHECK(f1.params.size() == 0);

  CHECK(s2.name == "Derived");
  CHECK(s2.briefComment == "");
  CHECK(s2.docComment == "");
  CHECK(s2.ID.str().size() == 16);
  CHECK(s2.parentNamespaceID.raw() == 0);

  CHECK(s2.type == "class");
  CHECK(s2.proto == "class Derived");
  CHECK(s2.vars.size() == 0);
  CHECK(s2.methodIDs.size() == 1);
  CHECK(s2.baseRecords.size() == 1);

  CHECK(s2.baseRecords[0].id == s1.ID);
  CHECK(s2.baseRecords[0].access == clang::AS_public);
  CHECK(s2.baseRecords[0].name == "Root");

  hdoc::types::FunctionSymbol f2 = index.functions.entries.at(s2.methodIDs[0]);
  CHECK(f2.name == "foo");
  CHECK(f2.briefComment == "");
  CHECK(f2.docComment == "");
  CHECK(f2.ID.str().size() == 16);
  CHECK(f2.parentNamespaceID == s2.ID);

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
  CHECK(f2.isCtorOrDtor == false);

  CHECK(f2.access == clang::AS_private);
  CHECK(f2.storageClass == clang::SC_None);
  CHECK(f2.refQualifier == clang::RQ_None);

  CHECK(f2.proto == "void foo()");
  CHECK(f2.returnType == "void");
  CHECK(f2.returnTypeDocComment == "");
  CHECK(f2.params.size() == 0);
}

TEST_CASE("Interface pure virtual") {
  const std::string code = R"(
    class IFoo {
      virtual void foo() = 0;
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 1, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "IFoo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.type == "class");
  CHECK(s.proto == "class IFoo");
  CHECK(s.vars.size() == 0);
  CHECK(s.methodIDs.size() == 1);
  CHECK(s.baseRecords.size() == 0);

  hdoc::types::FunctionSymbol f = index.functions.entries.at(s.methodIDs[0]);
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
  CHECK(f.isVirtual == true);
  CHECK(f.isVariadic == false);
  CHECK(f.isNoExcept == false);
  CHECK(f.hasTrailingReturn == false);
  CHECK(f.isCtorOrDtor == false);

  CHECK(f.access == clang::AS_private);
  CHECK(f.storageClass == clang::SC_None);
  CHECK(f.refQualifier == clang::RQ_None);

  CHECK(f.proto == "virtual void foo()");
  CHECK(f.returnType == "void");
  CHECK(f.returnTypeDocComment == "");
  CHECK(f.params.size() == 0);
}

TEST_CASE("Multiple base functions") {
  const std::string code = R"(
    struct Base0 {
      virtual ~Base0() { }
    };
    struct Base1 {
      virtual ~Base1() { }
    };
    struct Derived : Base0, Base1 {
      ~Derived() override { }
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 3, 3, 0, 0);

  std::optional<hdoc::types::RecordSymbol> o1 = findByName(index.records, "Base0");
  std::optional<hdoc::types::RecordSymbol> o2 = findByName(index.records, "Base1");
  std::optional<hdoc::types::RecordSymbol> o3 = findByName(index.records, "Derived");

  CHECK(o1);
  CHECK(o2);
  CHECK(o3);

  hdoc::types::RecordSymbol s1 = *o1;
  hdoc::types::RecordSymbol s2 = *o2;
  hdoc::types::RecordSymbol s3 = *o3;

  CHECK(s1.name == "Base0");
  CHECK(s1.briefComment == "");
  CHECK(s1.docComment == "");
  CHECK(s1.ID.str().size() == 16);
  CHECK(s1.parentNamespaceID.raw() == 0);

  CHECK(s1.type == "struct");
  CHECK(s1.proto == "struct Base0");
  CHECK(s1.vars.size() == 0);
  CHECK(s1.methodIDs.size() == 1);
  CHECK(s1.baseRecords.size() == 0);

  hdoc::types::FunctionSymbol f1 = index.functions.entries.at(s1.methodIDs[0]);
  CHECK(f1.name == "~Base0");
  CHECK(f1.briefComment == "");
  CHECK(f1.docComment == "");
  CHECK(f1.ID.str().size() == 16);
  CHECK(f1.parentNamespaceID == s1.ID);

  CHECK(f1.isRecordMember == true);
  CHECK(f1.isConstexpr == false);
  CHECK(f1.isConsteval == false);
  CHECK(f1.isInline == false);
  CHECK(f1.isConst == false);
  CHECK(f1.isVolatile == false);
  CHECK(f1.isRestrict == false);
  CHECK(f1.isVirtual == true);
  CHECK(f1.isVariadic == false);
  CHECK(f1.isNoExcept == false);
  CHECK(f1.hasTrailingReturn == false);
  CHECK(f1.isCtorOrDtor == true);

  CHECK(f1.access == clang::AS_public);
  CHECK(f1.storageClass == clang::SC_None);
  CHECK(f1.refQualifier == clang::RQ_None);

  CHECK(f1.proto == "virtual ~Base0()");
  CHECK(f1.returnType == "");
  CHECK(f1.returnTypeDocComment == "");
  CHECK(f1.params.size() == 0);

  CHECK(s2.name == "Base1");
  CHECK(s2.briefComment == "");
  CHECK(s2.docComment == "");
  CHECK(s2.ID.str().size() == 16);
  CHECK(s2.parentNamespaceID.raw() == 0);

  CHECK(s2.type == "struct");
  CHECK(s2.proto == "struct Base1");
  CHECK(s2.vars.size() == 0);
  CHECK(s2.methodIDs.size() == 1);
  CHECK(s2.baseRecords.size() == 0);

  hdoc::types::FunctionSymbol f2 = index.functions.entries.at(s2.methodIDs[0]);
  CHECK(f2.name == "~Base1");
  CHECK(f2.briefComment == "");
  CHECK(f2.docComment == "");
  CHECK(f2.ID.str().size() == 16);
  CHECK(f2.parentNamespaceID == s2.ID);

  CHECK(f2.isRecordMember == true);
  CHECK(f2.isConstexpr == false);
  CHECK(f2.isConsteval == false);
  CHECK(f2.isInline == false);
  CHECK(f2.isConst == false);
  CHECK(f2.isVolatile == false);
  CHECK(f2.isRestrict == false);
  CHECK(f2.isVirtual == true);
  CHECK(f2.isVariadic == false);
  CHECK(f2.isNoExcept == false);
  CHECK(f2.hasTrailingReturn == false);
  CHECK(f2.isCtorOrDtor == true);

  CHECK(f2.access == clang::AS_public);
  CHECK(f2.storageClass == clang::SC_None);
  CHECK(f2.refQualifier == clang::RQ_None);

  CHECK(f2.proto == "virtual ~Base1()");
  CHECK(f2.returnType == "");
  CHECK(f2.returnTypeDocComment == "");
  CHECK(f2.params.size() == 0);

  CHECK(s3.name == "Derived");
  CHECK(s3.briefComment == "");
  CHECK(s3.docComment == "");
  CHECK(s3.ID.str().size() == 16);
  CHECK(s3.parentNamespaceID.raw() == 0);

  CHECK(s3.type == "struct");
  CHECK(s3.proto == "struct Derived");
  CHECK(s3.vars.size() == 0);
  CHECK(s3.methodIDs.size() == 1);
  CHECK(s3.baseRecords.size() == 2);

  CHECK(s3.baseRecords[0].id == s1.ID);
  CHECK(s3.baseRecords[0].access == clang::AS_public);
  CHECK(s3.baseRecords[0].name == "Base0");

  CHECK(s3.baseRecords[1].id == s2.ID);
  CHECK(s3.baseRecords[1].access == clang::AS_public);
  CHECK(s3.baseRecords[1].name == "Base1");

  hdoc::types::FunctionSymbol f3 = index.functions.entries.at(s3.methodIDs[0]);
  CHECK(f3.name == "~Derived");
  CHECK(f3.briefComment == "");
  CHECK(f3.docComment == "");
  CHECK(f3.ID.str().size() == 16);
  CHECK(f3.parentNamespaceID == s3.ID);

  CHECK(f3.isRecordMember == true);
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
  CHECK(f3.isCtorOrDtor == true);

  CHECK(f3.access == clang::AS_public);
  CHECK(f3.storageClass == clang::SC_None);
  CHECK(f3.refQualifier == clang::RQ_None);

  CHECK(f3.proto == "~Derived()");
  CHECK(f3.returnType == "");
  CHECK(f3.returnTypeDocComment == "");
  CHECK(f3.params.size() == 0);
}
