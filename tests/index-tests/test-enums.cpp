// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "common.hpp"

TEST_CASE("Enum class decl") {
  const std::string code = R"(
    enum class Foo {
      A,
      B,
      C,
      D,
      E,
      F,
      G,
      H,
      I,
      J,
      K,
    };
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 1, 0);

  hdoc::types::EnumSymbol s = index.enums.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);
  CHECK(s.type == "enum class");
  CHECK(s.members.size() == 11);

  CHECK(s.members[0].value == 0);
  CHECK(s.members[0].name == "A");
  CHECK(s.members[0].docComment == "");

  CHECK(s.members[1].value == 1);
  CHECK(s.members[1].name == "B");
  CHECK(s.members[1].docComment == "");

  CHECK(s.members[2].value == 2);
  CHECK(s.members[2].name == "C");
  CHECK(s.members[2].docComment == "");

  CHECK(s.members[3].value == 3);
  CHECK(s.members[3].name == "D");
  CHECK(s.members[3].docComment == "");

  CHECK(s.members[4].value == 4);
  CHECK(s.members[4].name == "E");
  CHECK(s.members[4].docComment == "");

  CHECK(s.members[5].value == 5);
  CHECK(s.members[5].name == "F");
  CHECK(s.members[5].docComment == "");

  CHECK(s.members[6].value == 6);
  CHECK(s.members[6].name == "G");
  CHECK(s.members[6].docComment == "");

  CHECK(s.members[7].value == 7);
  CHECK(s.members[7].name == "H");
  CHECK(s.members[7].docComment == "");

  CHECK(s.members[8].value == 8);
  CHECK(s.members[8].name == "I");
  CHECK(s.members[8].docComment == "");

  CHECK(s.members[9].value == 9);
  CHECK(s.members[9].name == "J");
  CHECK(s.members[9].docComment == "");

  CHECK(s.members[10].value == 10);
  CHECK(s.members[10].name == "K");
  CHECK(s.members[10].docComment == "");
}

TEST_CASE("Enum class decl with uint8_t") {
  const std::string code = R"(
    typedef unsigned char uint8_t;
    enum class Foo : uint8_t {
      A,
      B = 20
    };
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 1, 0);

  hdoc::types::EnumSymbol s = index.enums.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);
  CHECK(s.type == "enum class");
  CHECK(s.members.size() == 2);

  CHECK(s.members[0].value == 0);
  CHECK(s.members[0].name == "A");
  CHECK(s.members[0].docComment == "");

  CHECK(s.members[1].value == 20);
  CHECK(s.members[1].name == "B");
  CHECK(s.members[1].docComment == "");
}

TEST_CASE("Enum struct decl") {
  const std::string code = R"(
    enum struct Foo {
      A,
      B = 20
    };
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 1, 0);

  hdoc::types::EnumSymbol s = index.enums.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);
  CHECK(s.type == "enum struct");
  CHECK(s.members.size() == 2);

  CHECK(s.members[0].value == 0);
  CHECK(s.members[0].name == "A");
  CHECK(s.members[0].docComment == "");

  CHECK(s.members[1].value == 20);
  CHECK(s.members[1].name == "B");
  CHECK(s.members[1].docComment == "");
}

TEST_CASE("Ordinary enum decl") {
  const std::string code = R"(
    enum Foo {
      A,
      B = 20
    };
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 1, 0);

  hdoc::types::EnumSymbol s = index.enums.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);
  CHECK(s.type == "enum");
  CHECK(s.members.size() == 2);

  CHECK(s.members[0].value == 0);
  CHECK(s.members[0].name == "A");
  CHECK(s.members[0].docComment == "");

  CHECK(s.members[1].value == 20);
  CHECK(s.members[1].name == "B");
  CHECK(s.members[1].docComment == "");
}
