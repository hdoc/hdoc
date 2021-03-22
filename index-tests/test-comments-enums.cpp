#include "common.hpp"

TEST_CASE("Enum class decl with comments") {
  const std::string code = R"(
    /// @brief aaa aaa aaa aaa
    ///
    /// bbb bbb bbb bbb
    enum class Foo {
        A,
        B,
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 0, 1, 0);

  hdoc::types::EnumSymbol s = index.enums.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "aaa aaa aaa aaa");
  CHECK(s.docComment == "bbb bbb bbb bbb");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);
  CHECK(s.type == "enum class");
  CHECK(s.members.size() == 2);

  CHECK(s.members[0].value == 0);
  CHECK(s.members[0].name == "A");
  CHECK(s.members[0].docComment == "");

  CHECK(s.members[1].value == 1);
  CHECK(s.members[1].name == "B");
  CHECK(s.members[1].docComment == "");
}

TEST_CASE("Enum class decl with comments") {
  const std::string code = R"(
    /// aaa aaa aaa aaa
    ///
    /// bbb bbb bbb bbb
    enum class Foo {
        A,
        B,
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 0, 1, 0);

  hdoc::types::EnumSymbol s = index.enums.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "aaa aaa aaa aaa bbb bbb bbb bbb");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);
  CHECK(s.type == "enum class");
  CHECK(s.members.size() == 2);

  CHECK(s.members[0].value == 0);
  CHECK(s.members[0].name == "A");
  CHECK(s.members[0].docComment == "");

  CHECK(s.members[1].value == 1);
  CHECK(s.members[1].name == "B");
  CHECK(s.members[1].docComment == "");
}

TEST_CASE("Enum class decl with comments") {
  const std::string code = R"(
    /// @brief foo bar baz
    enum class Foo {
        /// foo
        A = 0x00,
        /// bar
        B = 0x01,
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 0, 1, 0);

  hdoc::types::EnumSymbol s = index.enums.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "foo bar baz");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);
  CHECK(s.type == "enum class");
  CHECK(s.members.size() == 2);

  CHECK(s.members[0].value == 0x00);
  CHECK(s.members[0].name == "A");
  CHECK(s.members[0].docComment == "foo");

  CHECK(s.members[1].value == 0x01);
  CHECK(s.members[1].name == "B");
  CHECK(s.members[1].docComment == "bar");
}

TEST_CASE("Enum class decl with long doc comment") {
  const std::string code = R"(
    /// @brief foo bar baz
    ///
    /// Lorem ipsum dolor sit amet, consectetur adipiscing elit.
    /// Ut ultricies, elit non laoreet sodales, nibh velit lacinia
    /// nulla, ultricies finibus ex diam eget erat. Vestibulum mattis
    /// neque quis neque eleifend.
    enum class Foo {
        /// foo
        A = 0x00,
        /// bar
        B = 0x01,
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 0, 1, 0);

  hdoc::types::EnumSymbol s = index.enums.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "foo bar baz");
  CHECK(
      s.docComment ==
      "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut ultricies, elit non laoreet sodales, nibh velit lacinia nulla, ultricies finibus ex diam eget erat. Vestibulum mattis neque quis neque eleifend.");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);
  CHECK(s.type == "enum class");
  CHECK(s.members.size() == 2);

  CHECK(s.members[0].value == 0x00);
  CHECK(s.members[0].name == "A");
  CHECK(s.members[0].docComment == "foo");

  CHECK(s.members[1].value == 0x01);
  CHECK(s.members[1].name == "B");
  CHECK(s.members[1].docComment == "bar");
}

TEST_CASE("Enum class decl with comments (alternate style)") {
  const std::string code = R"(
    /// \brief foo bar baz
    enum struct Foo {
        A = 0x00, ///< foo
        B = 0x01, ///< bar
    };
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 0, 0, 1, 0);

  hdoc::types::EnumSymbol s = index.enums.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "foo bar baz");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);
  CHECK(s.type == "enum struct");
  CHECK(s.members.size() == 2);

  CHECK(s.members[0].value == 0x00);
  CHECK(s.members[0].name == "A");
  CHECK(s.members[0].docComment == "foo");

  CHECK(s.members[1].value == 0x01);
  CHECK(s.members[1].name == "B");
  CHECK(s.members[1].docComment == "bar");
}
