// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "serde/JSONDeserializer.hpp"
#include "serde/JSONSerializer.hpp"
#include "tests/TestUtils.hpp"

#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"

TEST_CASE("Check if NamespaceSymbol is the same after serde roundtrip") {
  std::vector<std::string> inputs = {
      R"(
        /**
         *  @brief foo bar baz
         *
         */
        namespace foo {}
      )",
      R"(
        /// foo bar baz
        namespace foo {}
      )",
      R"(
        namespace hello {
          void foo(int a, int b);
        }
      )",
      R"(
        namespace hello {
          class Foo {
            void foo();
          };
        }
      )",
      R"(
        namespace hello {
          class Foo {
            void foo();
          };

          void Foo::foo() {}
        }
      )",
      R"(
        namespace hello {
          class Foo {
            void foo() {}
          };
        }
      )",
      R"(
        namespace foo {
          namespace bar {
            void baz() {}
          }
        }
      )",
      R"(
        namespace foo {
            namespace bar {
                 namespace baz {
                     int qux = 42;
                 }
            }
        }

        namespace fbz = foo::bar::baz;
      )",
  };

  for (const std::string_view testCase : inputs) {
    hdoc::types::Index                               index;
    hdoc::types::Config                              cfg;
    rapidjson::StringBuffer                          buf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);

    runOverCode(testCase, index);
    hdoc::types::NamespaceSymbol s = index.namespaces.entries.begin()->second;

    hdoc::serde::JSONSerializer jsonSerializer(&index, &cfg);
    jsonSerializer.serializeNamespace(s, writer);
    std::string serializedToJSON = buf.GetString();

    rapidjson::Document document;
    document.Parse(serializedToJSON);
    hdoc::serde::JSONDeserializer      jsonDeserializer;
    const hdoc::types::NamespaceSymbol s2 = jsonDeserializer.deserializeNamespaceSymbol(document);

    CHECK(s == s2);
  }
}
