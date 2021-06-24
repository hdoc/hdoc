// Copyright 2019-2021 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.hpp"
#include "getBareTypeTestCases.hpp"
#include "serde/HTMLWriter.hpp"

#include <string>
#include <vector>

TEST_CASE("Testing getBareTypeName on a huge set of test cases") {
  for (const auto& testCase : getBareTypeTestCases) {
    CHECK(hdoc::serde::getBareTypeName(testCase.input) == testCase.expected);
  }
}

TEST_CASE("Testing getHyperlinkedFunctionProto") {
  struct TestCase {
    const char*                                   input;
    const char*                                   output;
    const std::vector<hdoc::types::FunctionParam> params;
    const hdoc::types::TypeRef                    returnType;
  };

  const std::vector<TestCase> cases = {
      {
          "void f()",
          "void f()",
          {},
          hdoc::types::TypeRef(),
      },
      {
          "int f()",
          "int f()",
          {},
          {hdoc::types::SymbolID(), "int"},
      },
      {
          "A f()",
          R"(<a href="rB6589FC6AB0DC82C.html">A</a> f())",
          {},
          {hdoc::types::SymbolID("0"), "A"},
      },
      {
          "std::string f()",
          R"(<a href="https://en.cppreference.com/w/cpp/string/basic_string">std::string</a> f())",
          {},
          {hdoc::types::SymbolID(), "std::string"},
      },
      {
          "std::vector<int> f()",
          R"(<a href="https://en.cppreference.com/w/cpp/container/vector">std::vector</a>&lt;int&gt; f())",
          {},
          {hdoc::types::SymbolID(), "std::vector<int>"},
      },
      {
          "std::vector<int *> f()",
          R"(<a href="https://en.cppreference.com/w/cpp/container/vector">std::vector</a>&lt;int*&gt; f())",
          {},
          {hdoc::types::SymbolID(), "std::vector<int *>"},
      },
      {
          "template <typename T> std::vector<T> f()",
          R"(template &lt;typename T&gt;
<a href="https://en.cppreference.com/w/cpp/container/vector">std::vector</a>&lt;T&gt; f())",
          {},
          {hdoc::types::SymbolID(), "std::vector<T>"},
      },
      {
          "template <typename T> A<T> f()",
          R"(template &lt;typename T&gt;
<a href="rB6589FC6AB0DC82C.html">A</a>&lt;T&gt; f())",
          {},
          {hdoc::types::SymbolID("0"), "A<T>"},
      },
      {
          "void f(int i)",
          "void f(int i)",
          {
              {"i", {hdoc::types::SymbolID(), "int"}, "", ""},
          },
          hdoc::types::TypeRef(),
      },
      {
          "void f(const int i)",
          "void f(const int i)",
          {
              {"i", {hdoc::types::SymbolID(), "const int"}, "", ""},
          },
          hdoc::types::TypeRef(),
      },
      {
          "void f(const int & i)",
          "void f(const int&amp; i)",
          {
              {"i", {hdoc::types::SymbolID(), "const int &"}, "", ""},
          },
          hdoc::types::TypeRef(),
      },
      {
          "void f(A & i)",
          R"(void f(<a href="rB6589FC6AB0DC82C.html">A</a>&amp; i))",
          {
              {"i", {hdoc::types::SymbolID("0"), "A &"}, "", ""},
          },
          hdoc::types::TypeRef(),
      },
      {
          "void f(std::string i)",
          R"(void f(<a href="https://en.cppreference.com/w/cpp/string/basic_string">std::string</a> i))",
          {
              {"i", {hdoc::types::SymbolID(), "std::string"}, "", ""},
          },
          hdoc::types::TypeRef(),
      },
      {
          "void f(std::vector<int> i)",
          R"(void f(<a href="https://en.cppreference.com/w/cpp/container/vector">std::vector</a>&lt;int&gt; i))",
          {
              {"i", {hdoc::types::SymbolID(), "std::vector<int>"}, "", ""},
          },
          hdoc::types::TypeRef(),
      },
      {
          "template <typename T> void f(A<T> i)",
          R"(template &lt;typename T&gt;
void f(<a href="rB6589FC6AB0DC82C.html">A</a>&lt;T&gt; i))",
          {
              {"i", {hdoc::types::SymbolID("0"), "A<T>"}, "", ""},
          },
          hdoc::types::TypeRef(),
      },
  };

  for (const auto& testCase : cases) {
    hdoc::types::FunctionSymbol f;
    f.name           = "f";
    f.params         = testCase.params;
    f.returnType     = testCase.returnType;
    const auto proto = hdoc::serde::clangFormat(testCase.input);
    CHECK(hdoc::serde::getHyperlinkedFunctionProto(proto, f) == std::string(testCase.output));
  }
}
