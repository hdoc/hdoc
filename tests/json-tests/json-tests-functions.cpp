// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "serde/JSONDeserializer.hpp"
#include "serde/JSONSerializer.hpp"
#include "tests/TestUtils.hpp"

#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

TEST_CASE("Check if FunctionSymbol is the same after serde roundtrip") {
  std::vector<std::string> inputs = {
      R"(
        struct Foo;
        void foo(Foo* p0, Foo* p1) {}
      )",
      R"(
        void foo(int, int) {}
      )",
      R"(
        void foo(int a = 0, int b = 100) {}
      )",
      R"(
        auto foo(int x, int y) -> int;
      )",
      R"(
        constexpr int gcd(int a, int b){
          return (b == 0) ? a : gcd(b, a % b);
        }
      )",
      R"(
        struct Foo {
          void bar() volatile;
        };
      )",
      R"(
        struct Foo {
          void get() &;
        };
      )",
      R"(
        struct Foo {
          void get() &&;
        };
      )",
      R"(
        struct Foo {
          void get() const &;
        };
      )",
      R"(
        struct Foo {
          void get() const &&;
        };
      )",
      R"(
        void foo() noexcept;
      )",
      R"(
        void foo() noexcept(true);
      )",
      R"(
        void simple_printf(const char* fmt...);
      )",
      R"(
        inline int cube(int s) {
            return s*s*s;
        }
      )",
      R"(
        class Type {
        public:
          constexpr auto take() && noexcept -> Type;
        };
      )",
      R"(
        class Type {
        public:
          constexpr auto borrow() const& noexcept -> const Type&;
        };
      )",
      R"(
        /// Some comment
        void someFunction();
      )",
      R"(
        /**
         * Some comment
         */
        void someFunction();
      )",
      R"(
        void someFunction(); ///< Some comment
      )",
      R"(
        /// @brief does foo to x and y
        ///
        /// @param x bar
        /// @param y baz
        /// @returns boo
        auto foo(int x, int y) -> int;
      )",
      R"(
        /// @brief does foo to x and y
        ///
        /// @param x bar
        /// @param y baz
        /// @param z nonexistent
        /// @returns boo
        auto foo(int x, int y) -> int;
      )",
      R"(
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
      )",
      R"(
        /// Calculate Euclidean distance in $\R^2$.
        /// Corresponds to the following formula:
        /// $$ d(x,y) = \sqrt{(x_2-x_1)^2 + (y_2-y_1)^2} $$
        /// @param x1 $\sqrt{x_1}$
        /// @param y1 $\sqrt{y_1}$
        /// @param x2 $\sqrt{x_2}$
        /// @param y2 $\sqrt{y_2}$
        /// @returns the result of $$ \sqrt{(x_2-x_1)^2 + (y_2-y_1)^2} $$
        double calculate2DEuclideanDistance(const double x1, const double y1, const double x2, const double y2);
      )",
      R"(
        /// @brief does foo to x
        ///
        /// @param x bar
        /// @param
        /// @returns boo
        int foo(int x);
      )",
      R"(
        /// @brief Testing if inline command comments, like @a varX, work.
        ///
        /// Let's see if they work in docComments @b makeMeBold.
        int foo(int varX);
      )",
      R"(
        /// Given input @a foo and then..
        void abc(int foo);
      )",
      R"(
        /// Unicode in a comment: ✓ testing...
        void abc(int foo);
      )",
  };

  for (const std::string_view testCase : inputs) {
    hdoc::types::Index                               index;
    hdoc::types::Config                              cfg;
    rapidjson::StringBuffer                          buf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);

    runOverCode(testCase, index);
    hdoc::types::FunctionSymbol s = index.functions.entries.begin()->second;

    hdoc::serde::JSONSerializer jsonSerializer(&index, &cfg);
    jsonSerializer.serializeFunction(s, writer);
    std::string serializedToJSON = buf.GetString();

    rapidjson::Document document;
    document.Parse(serializedToJSON);
    hdoc::serde::JSONDeserializer     jsonDeserializer;
    const hdoc::types::FunctionSymbol s2 = jsonDeserializer.deserializeFunctionSymbol(document);

    CHECK(s == s2);
  }
}
