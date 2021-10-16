// Copyright 2019-2021 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "common.hpp"

TEST_CASE("Function with docComment only (style 1)") {
  const std::string code = R"(
    /// Some comment
    void someFunction();
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "someFunction");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "Some comment");
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

  CHECK(s.proto == "void someFunction()");
  CHECK(s.returnType.name == "void");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == "");
  CHECK(s.params.size() == 0);
}

TEST_CASE("Function with docComment only (style 2)") {
  const std::string code = R"(
    /**
     * Some comment
     */
    void someFunction();
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "someFunction");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "Some comment");
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

  CHECK(s.proto == "void someFunction()");
  CHECK(s.returnType.name == "void");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == "");
  CHECK(s.params.size() == 0);
}

TEST_CASE("Function with docComment only (style 3, ignored)") {
  const std::string code = R"(
    void someFunction(); ///< Some comment
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "someFunction");
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

  CHECK(s.proto == "void someFunction()");
  CHECK(s.returnType.name == "void");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == "");
  CHECK(s.params.size() == 0);
}

TEST_CASE("Function with trailing return type syntax and comments") {
  const std::string code = R"(
    /// @brief does foo to x and y
    ///
    /// @param x bar
    /// @param y baz
    /// @returns boo
    auto foo(int x, int y) -> int;
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "foo");
  CHECK(s.briefComment == "does foo to x and y");
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
  CHECK(s.returnTypeDocComment == "boo");

  CHECK(s.params.size() == 2);
  CHECK(s.params[0].name == "x");
  CHECK(s.params[0].type.name == "int");
  CHECK(s.params[0].type.id.raw() == 0);
  CHECK(s.params[0].docComment == "bar");
  CHECK(s.params[0].defaultValue == "");

  CHECK(s.params[1].name == "y");
  CHECK(s.params[1].type.name == "int");
  CHECK(s.params[1].type.id.raw() == 0);
  CHECK(s.params[1].docComment == "baz");
  CHECK(s.params[1].defaultValue == "");
}

TEST_CASE("Function with trailing return type syntax, comments, and an extra errant comment") {
  const std::string code = R"(
    /// @brief does foo to x and y
    ///
    /// @param x bar
    /// @param y baz
    /// @param z nonexistent
    /// @returns boo
    auto foo(int x, int y) -> int;
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "foo");
  CHECK(s.briefComment == "does foo to x and y");
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
  CHECK(s.returnTypeDocComment == "boo");

  CHECK(s.params.size() == 2);
  CHECK(s.params[0].name == "x");
  CHECK(s.params[0].type.name == "int");
  CHECK(s.params[0].type.id.raw() == 0);
  CHECK(s.params[0].docComment == "bar");
  CHECK(s.params[0].defaultValue == "");

  CHECK(s.params[1].name == "y");
  CHECK(s.params[1].type.name == "int");
  CHECK(s.params[1].type.id.raw() == 0);
  CHECK(s.params[1].docComment == "baz");
  CHECK(s.params[1].defaultValue == "");
}

TEST_CASE("Function that uses many unsupported doxygen commands") {
  const std::string code = R"(
    /// @brief Add curve x[i], y[i] to chart
    ///
    /// @pre  Precondition: the arrays x[] and y[] must have size n.
    /// @post There are no post conditions.
    ///
    /// @param n  array size
    /// @param x  array of x-coordinates values
    /// @param y  array of y-coordinates values
    /// @return   Void
    ///
    /// @details
    /// Plot the curve comprised of points P[i] = (X[i], Y[i]),
    /// where i = 0, 1, 2... n - 1.
    ///
    void addCurve(int n, const double x[], const double y[]);
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "addCurve");
  CHECK(s.briefComment == "Add curve x[i], y[i] to chart");
  CHECK(s.docComment == ""); // TODO: this should pick up the @details comments
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

  CHECK(s.proto == "void addCurve(int n, const double * x, const double * y)");
  CHECK(s.returnType.name == "void");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == "Void");

  CHECK(s.params.size() == 3);
  CHECK(s.params[0].name == "n");
  CHECK(s.params[0].type.name == "int");
  CHECK(s.params[0].type.id.raw() == 0);
  CHECK(s.params[0].docComment == "array size");
  CHECK(s.params[0].defaultValue == "");

  CHECK(s.params[1].name == "x");
  CHECK(s.params[1].type.name == "const double *");
  CHECK(s.params[1].type.id.raw() == 0);
  CHECK(s.params[1].docComment == "array of x-coordinates values");
  CHECK(s.params[1].defaultValue == "");

  CHECK(s.params[2].name == "y");
  CHECK(s.params[2].type.name == "const double *");
  CHECK(s.params[2].type.id.raw() == 0);
  CHECK(s.params[2].docComment == "array of y-coordinates values");
  CHECK(s.params[2].defaultValue == "");
}

TEST_CASE("Function has math commands in function and parameter names") {
  const std::string code = R"(
    /// Calculate Euclidean distance in $\R^2$.
    /// Corresponds to the following formula:
    /// $$ d(x,y) = \sqrt{(x_2-x_1)^2 + (y_2-y_1)^2} $$
    /// @param x1 $\sqrt{x_1}$
    /// @param y1 $\sqrt{y_1}$
    /// @param x2 $\sqrt{x_2}$
    /// @param y2 $\sqrt{y_2}$
    /// @returns the result of $$ \sqrt{(x_2-x_1)^2 + (y_2-y_1)^2} $$
    double calculate2DEuclideanDistance(const double x1, const double y1, const double x2, const double y2);
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 1, 0, 0);

  hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;
  CHECK(s.name == "calculate2DEuclideanDistance");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == R"(Calculate Euclidean distance in $\R^2$. Corresponds to the following formula: $$ d(x,y) = \sqrt{(x_2-x_1)^2 + (y_2-y_1)^2} $$)");
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

  CHECK(s.proto == "double calculate2DEuclideanDistance(const double x1, const double y1, const double x2, const double y2)");
  CHECK(s.returnType.name == "double");
  CHECK(s.returnType.id.raw() == 0);
  CHECK(s.returnTypeDocComment == R"(the result of $$ \sqrt{(x_2-x_1)^2 + (y_2-y_1)^2} $$)");

  CHECK(s.params.size() == 4);
  CHECK(s.params[0].name == "x1");
  CHECK(s.params[0].type.name == "const double");
  CHECK(s.params[0].type.id.raw() == 0);
  CHECK(s.params[0].docComment == R"($\sqrt{x_1}$)");
  CHECK(s.params[0].defaultValue == "");

  CHECK(s.params[1].name == "y1");
  CHECK(s.params[1].type.name == "const double");
  CHECK(s.params[1].type.id.raw() == 0);
  CHECK(s.params[1].docComment == R"($\sqrt{y_1}$)");
  CHECK(s.params[1].defaultValue == "");

  CHECK(s.params[2].name == "x2");
  CHECK(s.params[2].type.name == "const double");
  CHECK(s.params[2].type.id.raw() == 0);
  CHECK(s.params[2].docComment == R"($\sqrt{x_2}$)");
  CHECK(s.params[2].defaultValue == "");

  CHECK(s.params[3].name == "y2");
  CHECK(s.params[3].type.name == "const double");
  CHECK(s.params[3].type.id.raw() == 0);
  CHECK(s.params[3].docComment == R"($\sqrt{y_2}$)");
  CHECK(s.params[3].defaultValue == "");
}
