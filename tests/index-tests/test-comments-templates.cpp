// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "common.hpp"

TEST_CASE("Test a function with tparam comments") {
  const std::string code = R"(
    /// \brief decoy brief comment
    /// \tparam T a test comment
    /// \return decoy return comment
    template <typename T> void f(T s) {
      (void)s;
    }
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "f");
  CHECK(s.briefComment == "decoy brief comment");
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

  CHECK(s.proto == "template <typename T>void f(T s)");
  CHECK(s.returnType.name == "void");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == "decoy return comment");
  CHECK(s.params.size() == 1);

  CHECK(s.templateParams.size() == 1);
  CHECK(s.templateParams[0].templateType == hdoc::types::TemplateParam::TemplateType::TemplateTypeParameter);
  CHECK(s.templateParams[0].name == "T");
  CHECK(s.templateParams[0].type == "");
  CHECK(s.templateParams[0].docComment == "a test comment");
  CHECK(s.templateParams[0].defaultValue == "");
  CHECK(s.templateParams[0].isParameterPack == false);
  CHECK(s.templateParams[0].isTypename == true);
}

TEST_CASE("Test a function with multiple tparam comments") {
  const std::string code = R"(
    /// \brief decoy brief comment
    /// \tparam T comment1
    /// \tparam U comment2
    /// \return decoy return comment
    template <typename T, typename U> void f(T s) {
      (void)s;
    }
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "f");
  CHECK(s.briefComment == "decoy brief comment");
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

  CHECK(s.proto == "template <typename T, typename U>void f(T s)");
  CHECK(s.returnType.name == "void");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == "decoy return comment");
  CHECK(s.params.size() == 1);

  CHECK(s.templateParams.size() == 2);
  CHECK(s.templateParams[0].templateType == hdoc::types::TemplateParam::TemplateType::TemplateTypeParameter);
  CHECK(s.templateParams[0].name == "T");
  CHECK(s.templateParams[0].type == "");
  CHECK(s.templateParams[0].docComment == "comment1");
  CHECK(s.templateParams[0].defaultValue == "");
  CHECK(s.templateParams[0].isParameterPack == false);
  CHECK(s.templateParams[0].isTypename == true);

  CHECK(s.templateParams[1].templateType == hdoc::types::TemplateParam::TemplateType::TemplateTypeParameter);
  CHECK(s.templateParams[1].name == "U");
  CHECK(s.templateParams[1].type == "");
  CHECK(s.templateParams[1].docComment == "comment2");
  CHECK(s.templateParams[1].defaultValue == "");
  CHECK(s.templateParams[1].isParameterPack == false);
  CHECK(s.templateParams[1].isTypename == true);
}

TEST_CASE("Test a templated class with a tparam comment") {
  const std::string code = R"(
    /// \brief decoy brief comment
    /// \tparam T1 a comment
    /// \tparam T2 another comment
    /// \returns nothing
    template <class T1, class T2> class Test {};
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 0, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Test");
  CHECK(s.briefComment == "decoy brief comment");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.type == "class");
  CHECK(s.proto == "template <class T1, class T2> class Test");
  CHECK(s.vars.size() == 0);
  CHECK(s.methodIDs.size() == 0);
  CHECK(s.baseRecords.size() == 0);

  CHECK(s.templateParams.size() == 2);
  CHECK(s.templateParams[0].templateType == hdoc::types::TemplateParam::TemplateType::TemplateTypeParameter);
  CHECK(s.templateParams[0].name == "T1");
  CHECK(s.templateParams[0].type == "");
  CHECK(s.templateParams[0].docComment == "a comment");
  CHECK(s.templateParams[0].defaultValue == "");
  CHECK(s.templateParams[0].isParameterPack == false);
  CHECK(s.templateParams[0].isTypename == false);

  CHECK(s.templateParams[1].templateType == hdoc::types::TemplateParam::TemplateType::TemplateTypeParameter);
  CHECK(s.templateParams[1].name == "T2");
  CHECK(s.templateParams[1].type == "");
  CHECK(s.templateParams[1].docComment == "another comment");
  CHECK(s.templateParams[1].defaultValue == "");
  CHECK(s.templateParams[1].isParameterPack == false);
  CHECK(s.templateParams[1].isTypename == false);
}

TEST_CASE("Test a templated class with multiple tparam comments") {
  const std::string code = R"(
    /// \brief decoy brief comment
    /// \tparam T1 a comment
    /// \tparam T2 another comment
    /// \returns nothing
    template <class T1, class T2> class Test {};
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 0, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Test");
  CHECK(s.briefComment == "decoy brief comment");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);

  CHECK(s.type == "class");
  CHECK(s.proto == "template <class T1, class T2> class Test");
  CHECK(s.vars.size() == 0);
  CHECK(s.methodIDs.size() == 0);
  CHECK(s.baseRecords.size() == 0);

  CHECK(s.templateParams.size() == 2);
  CHECK(s.templateParams[0].templateType == hdoc::types::TemplateParam::TemplateType::TemplateTypeParameter);
  CHECK(s.templateParams[0].name == "T1");
  CHECK(s.templateParams[0].type == "");
  CHECK(s.templateParams[0].docComment == "a comment");
  CHECK(s.templateParams[0].defaultValue == "");
  CHECK(s.templateParams[0].isParameterPack == false);
  CHECK(s.templateParams[0].isTypename == false);

  CHECK(s.templateParams[1].templateType == hdoc::types::TemplateParam::TemplateType::TemplateTypeParameter);
  CHECK(s.templateParams[1].name == "T2");
  CHECK(s.templateParams[1].type == "");
  CHECK(s.templateParams[1].docComment == "another comment");
  CHECK(s.templateParams[1].defaultValue == "");
  CHECK(s.templateParams[1].isParameterPack == false);
  CHECK(s.templateParams[1].isTypename == false);
}

TEST_CASE("Test a templated class with an empty tparam comment") {
  const std::string code = R"(
    /// \brief decoy brief comment
    /// \tparam T1 a comment
    /// \tparam
    template <class T1> class Test {};
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 0, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Test");
  CHECK(s.templateParams.size() == 1);
}
