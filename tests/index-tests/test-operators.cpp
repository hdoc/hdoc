// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "common.hpp"

TEST_CASE("Class with custom operators") {
  const std::string code = R"(
    class Foo {
      void operator()(int) {}
      void operator()(bool);
      int  operator()(int a, int b);
    };

    Foo &operator += (const Foo&, const int&);
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 0, 0, 0);
}
