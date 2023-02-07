// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "serde/JSONDeserializer.hpp"
#include "serde/JSONSerializer.hpp"
#include "tests/TestUtils.hpp"

#include <string>
#include <vector>

#include "doctest.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

TEST_CASE("Check if RecordSymbol is the same after serde roundtrip") {
  std::vector<std::string> inputs = {
      R"(
        class Foo {
          Foo* member;
        };
      )",
      R"(
        class Foo {
          Foo* a = nullptr;
          int  b = 10;
        };
      )",
      R"(
        class Foo {
          static Foo* member;
        };
      )",
      R"(
        class Foo {
          void foo(const int a) const {}
        };
      )",
      R"(
        struct Foo {
        private:
          void m1() {}
          int v1;
        public:
          void m2() {}
          int v2;
        };
      )",
      R"(
        struct Foo {
        private:
          void m1() {}
          int v1;
        public:
          void m2() {}
          int v2;
        };
      )",
      R"(
        /*!
         * @brief foo bar baz
         */
        class Foo {
          public:
            /// the sample rate (as integer 0..100)
            int m_sample_rate;
            /// whether the client is enabled
            bool m_enabled = true;
          private:
            /// the public key to be used in requests
            int m_public_key;
            /// the secret key to be used in requests
            int m_secret_key;
        };
      )",
      R"(
        /// @brief Testing if inline command comments, like @a varX, work.
        ///
        /// Let's see if they work in docComments @b makeMeBold.
        class Foo {
          public:
            /// the sample rate (as integer 0..100) @b makeMeBold2
            int m_sample_rate;
          private:
            /// the public key to be used in requests
            int m_public_key;
        };
      )",
  };

  for (const std::string_view testCase : inputs) {
    hdoc::types::Index                               index;
    hdoc::types::Config                              cfg;
    rapidjson::StringBuffer                          buf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);

    runOverCode(testCase, index);
    hdoc::types::RecordSymbol s = index.records.entries.begin()->second;

    hdoc::serde::JSONSerializer jsonSerializer(&index, &cfg);
    jsonSerializer.serializeRecord(s, writer);
    std::string serializedToJSON = buf.GetString();

    rapidjson::Document document;
    document.Parse(serializedToJSON);
    hdoc::serde::JSONDeserializer   jsonDeserializer;
    const hdoc::types::RecordSymbol s2 = jsonDeserializer.deserializeRecordSymbol(document);

    CHECK(s == s2);
  }
}
