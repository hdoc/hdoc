// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "serde/JSONDeserializer.hpp"
#include "serde/JSONSerializer.hpp"
#include "tests/TestUtils.hpp"

#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

TEST_CASE("Check if EnumSymbol is the same after serde roundtrip") {
  std::vector<std::string> inputs = {
      R"(
        /// @brief aaa aaa aaa aaa
        ///
        /// bbb bbb bbb bbb
        enum class Foo {
            A,
            B,
        };
      )",
      R"(
        /// aaa aaa aaa aaa
        ///
        /// bbb bbb bbb bbb
        enum class Foo {
            A,
            B,
        };
      )",
      R"(
        /// @brief foo bar baz
        enum class Foo {
            /// foo
            A = 0x00,
            /// bar
            B = 0x01,
        };
      )",
      R"(
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
      )",
      R"(
        /// \brief foo bar baz
        enum struct Foo {
            A = 0x00, ///< foo
            B = 0x01, ///< bar
        };
      )",
      R"(
        /// @brief Testing if inline command comments, like @a varX, work.
        ///
        /// Let's see if they work in docComments @b makeMeBold.
        enum class Foo {
            /// foo
            A = 0x00,
            /// bar
            B = 0x01,
        };
      )",
      R"(
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
      )",
      R"(
        typedef unsigned char uint8_t;
        enum class Foo : uint8_t {
          A,
          B = 20
        };
      )",
      R"(
        enum struct Foo {
          A,
          B = 20
        };
      )",
      R"(
        enum Foo {
          A,
          B = 20
        };
      )",
  };

  for (const std::string_view testCase : inputs) {
    hdoc::types::Index                               index;
    hdoc::types::Config                              cfg;
    rapidjson::StringBuffer                          buf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);

    runOverCode(testCase, index);
    hdoc::types::EnumSymbol s = index.enums.entries.begin()->second;

    hdoc::serde::JSONSerializer jsonSerializer(&index, &cfg);
    jsonSerializer.serializeEnum(s, writer);
    std::string serializedToJSON = buf.GetString();

    rapidjson::Document document;
    document.Parse(serializedToJSON);
    hdoc::serde::JSONDeserializer jsonDeserializer;
    const hdoc::types::EnumSymbol s2 = jsonDeserializer.deserializeEnumSymbol(document);

    CHECK(s == s2);
  }
}
