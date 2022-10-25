// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "serde/JSONDeserializer.hpp"
#include "tests/TestUtils.hpp"

#include <optional>
#include <string>
#include <vector>

#include "rapidjson/document.h"

std::optional<rapidjson::Document> parseJSON(const std::string json) {
  rapidjson::Document doc;

  if (doc.Parse(json).HasParseError()) {
    return std::nullopt;
  }

  return doc;
}

TEST_CASE("Check if a blantanly invalid JSON payload fails to parse") {
  const std::string json = R"(
      {
        "blabla": 1
      }
    )";
  const auto        doc  = parseJSON(json);
  REQUIRE(doc != std::nullopt);
  hdoc::serde::JSONDeserializer jsonDeserializer;
  CHECK(jsonDeserializer.validateJSON(*doc) == false);
}

TEST_CASE("Check if a JSON payload with all the properties but with the wrong types fails to parse") {
  const std::string json = R"(
      {
        "config": [],
        "index": [],
        "markdownFiles": []
      }
    )";
  const auto        doc  = parseJSON(json);
  REQUIRE(doc != std::nullopt);
  hdoc::serde::JSONDeserializer jsonDeserializer;
  CHECK(jsonDeserializer.validateJSON(*doc) == false);
}

TEST_CASE("Check if a JSON payload with incomplete config fails to parse") {
  const std::string json = R"(
      {
        "index": {
          "functions": [],
          "records": [],
          "enums": [],
          "namespaces": []
        },
        "config": {

        },
        "markdownFiles": []
      }
    )";
  const auto        doc  = parseJSON(json);
  REQUIRE(doc != std::nullopt);
  hdoc::serde::JSONDeserializer jsonDeserializer;
  CHECK(jsonDeserializer.validateJSON(*doc) == false);
}

TEST_CASE("Check if a record with duplicate methodIDs fails to pass validation") {
  const std::string json = R"(
    {
        "config": {
            "projectName": "hdoc",
            "timestamp": "2022-10-19T07:13:50 UTC",
            "hdocVersion": "1.3.2-hdocInternal",
            "gitRepoURL": "https://github.com/hdoc/hdoc/",
            "gitDefaultBranch": "master",
            "binaryType": 0
        },
        "index": {
            "functions": [],
            "records": [
                {
                    "id": 7979351357350575674,
                    "name": "BundledFile",
                    "docComment": "",
                    "briefComment": "",
                    "file": "src/serde/HTMLWriter.cpp",
                    "line": 81,
                    "parentNamespaceID": 0,
                    "type": "struct",
                    "proto": "struct BundledFile",
                    "vars": [
                        {
                            "isStatic": false,
                            "name": "len",
                            "type": {
                                "id": 0,
                                "name": "const unsigned int"
                            },
                            "defaultValue": "",
                            "docComment": "",
                            "access": 0
                        }
                    ],
                    "methodIDs": [1, 1],
                    "baseRecords": [],
                    "templateParams": []
                }
            ],
            "enums": [],
            "namespaces": []
        },
        "markdownFiles": []
    }
    )";
  const auto        doc  = parseJSON(json);
  REQUIRE(doc != std::nullopt);
  hdoc::serde::JSONDeserializer jsonDeserializer;
  CHECK(jsonDeserializer.validateJSON(*doc) == false);
}

TEST_CASE("Check if a well-formed JSON payload parses without failing") {
  const std::string json = R"(
    {
        "config": {
            "projectName": "hdoc",
            "timestamp": "2022-10-19T07:13:50 UTC",
            "hdocVersion": "1.3.2-hdocInternal",
            "gitRepoURL": "https://github.com/hdoc/hdoc/blob/master/",
            "gitDefaultBranch": "master",
            "binaryType": 0
        },
        "index": {
            "functions": [
                {
                    "id": 1851191799612219905,
                    "name": "printEnum",
                    "docComment": "",
                    "briefComment": "",
                    "file": "src/serde/JSONWriter.hpp",
                    "line": 261,
                    "parentNamespaceID": 17890685346955031521,
                    "isRecordMember": true,
                    "isConstexpr": false,
                    "isConsteval": false,
                    "isInline": false,
                    "isConst": true,
                    "isVolatile": false,
                    "isRestrict": false,
                    "isVirtual": false,
                    "isVariadic": false,
                    "isNoExcept": false,
                    "hasTrailingReturn": false,
                    "isCtorOrDtor": false,
                    "nameStart": 31,
                    "postTemplate": 26,
                    "access": 0,
                    "storageClass": 0,
                    "refQualifier": 0,
                    "proto": "template <typename Writer>void printEnum(const hdoc::types::EnumSymbol & e, Writer & writer) const",
                    "returnTypeDocComment": "",
                    "returnType": {
                        "id": 0,
                        "name": "void"
                    },
                    "params": [
                        {
                            "name": "e",
                            "type": {
                                "id": 17501071257327998595,
                                "name": "const hdoc::types::EnumSymbol &"
                            },
                            "docComment": "",
                            "defaultValue": ""
                        },
                        {
                            "name": "writer",
                            "type": {
                                "id": 0,
                                "name": "Writer &"
                            },
                            "docComment": "",
                            "defaultValue": ""
                        }
                    ],
                    "templateParams": [
                        {
                            "templateType": 0,
                            "name": "Writer",
                            "type": "",
                            "docComment": "",
                            "isParameterPack": false,
                            "isTypename": true
                        }
                    ]
                }
            ],
            "records": [
                {
                    "id": 7979351357350575674,
                    "name": "BundledFile",
                    "docComment": "",
                    "briefComment": "",
                    "file": "src/serde/HTMLWriter.cpp",
                    "line": 81,
                    "parentNamespaceID": 0,
                    "type": "struct",
                    "proto": "struct BundledFile",
                    "vars": [
                        {
                            "isStatic": false,
                            "name": "len",
                            "type": {
                                "id": 0,
                                "name": "const unsigned int"
                            },
                            "defaultValue": "",
                            "docComment": "",
                            "access": 0
                        }
                    ],
                    "methodIDs": [],
                    "baseRecords": [],
                    "templateParams": []
                }
            ],
            "enums": [
                {
                    "id": 3230339518593317227,
                    "name": "BinaryType",
                    "docComment": "",
                    "briefComment": "Indicates the type of hdoc binary.",
                    "file": "src/types/Config.hpp",
                    "line": 13,
                    "parentNamespaceID": 7231662954137597064,
                    "members": [
                        {
                            "name": "Full",
                            "value": 0,
                            "docComment": "blabla"
                        }
                    ]
                }
            ],
            "namespaces": [
                {
                    "id": 3258916802053398224,
                    "name": "indexer",
                    "docComment": "",
                    "briefComment": "",
                    "file": "src/indexer/Indexer.hpp",
                    "line": 11,
                    "parentNamespaceID": 242134248639948678,
                    "records": [
                        15015969461216030317
                    ],
                    "namespaces": [
                        94094196506148208
                    ],
                    "enums": []
                }
            ]
        },
        "markdownFiles": [
            {
                "isHomepage": true,
                "filename": "README.md",
                "contents": "test"
            }
        ]
    }
    )";
  const auto        doc  = parseJSON(json);
  REQUIRE(doc != std::nullopt);
  hdoc::serde::JSONDeserializer jsonDeserializer;
  CHECK(jsonDeserializer.validateJSON(*doc) == true);
}
