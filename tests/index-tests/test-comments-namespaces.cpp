// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "common.hpp"

TEST_CASE("Namespace with ignored brief comment") {
  const std::string code = R"(
    /**
     *  @brief foo bar baz
     *
     */
    namespace foo {}
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 0, 1);

  hdoc::types::NamespaceSymbol s = index.namespaces.entries.begin()->second;
  CHECK(s.name == "foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);
}

TEST_CASE("Namespace with ignored comment") {
  const std::string code = R"(
    /// foo bar baz
    namespace foo {}
  )";

  hdoc::types::Index index;
  runOverCode(code, index);
  checkIndexSizes(index, 0, 0, 0, 1);

  hdoc::types::NamespaceSymbol s = index.namespaces.entries.begin()->second;
  CHECK(s.name == "foo");
  CHECK(s.briefComment == "");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);
}
