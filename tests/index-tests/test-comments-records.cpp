// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "common.hpp"

TEST_CASE("Record with commented member variables") {
  const std::string code = R"(
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
  )";

  const hdoc::types::Index index = runOverCode(code);
  checkIndexSizes(index, 1, 0, 0, 0);

  hdoc::types::RecordSymbol s = index.records.entries.begin()->second;
  CHECK(s.name == "Foo");
  CHECK(s.briefComment == "foo bar baz");
  CHECK(s.docComment == "");
  CHECK(s.ID.str().size() == 16);
  CHECK(s.parentNamespaceID.raw() == 0);
  CHECK(s.vars.size() == 4);

  CHECK(s.vars[0].isStatic == false);
  CHECK(s.vars[0].name == "m_sample_rate");
  CHECK(s.vars[0].type.name == "int");
  CHECK(s.vars[0].type.id.raw() == 0);
  CHECK(s.vars[0].defaultValue == "");
  CHECK(s.vars[0].docComment == "the sample rate (as integer 0..100)");
  CHECK(s.vars[0].access == clang::AS_public);

  CHECK(s.vars[1].isStatic == false);
  CHECK(s.vars[1].name == "m_enabled");
  CHECK(s.vars[1].type.name == "bool");
  CHECK(s.vars[1].type.id.raw() == 0);
  CHECK(s.vars[1].defaultValue == "true");
  CHECK(s.vars[1].docComment == "whether the client is enabled");
  CHECK(s.vars[1].access == clang::AS_public);

  CHECK(s.vars[2].isStatic == false);
  CHECK(s.vars[2].name == "m_public_key");
  CHECK(s.vars[2].type.name == "int");
  CHECK(s.vars[2].type.id.raw() == 0);
  CHECK(s.vars[2].defaultValue == "");
  CHECK(s.vars[2].docComment == "the public key to be used in requests");
  CHECK(s.vars[2].access == clang::AS_private);

  CHECK(s.vars[3].isStatic == false);
  CHECK(s.vars[3].name == "m_secret_key");
  CHECK(s.vars[3].type.name == "int");
  CHECK(s.vars[3].type.id.raw() == 0);
  CHECK(s.vars[3].defaultValue == "");
  CHECK(s.vars[3].docComment == "the secret key to be used in requests");
  CHECK(s.vars[3].access == clang::AS_private);
}
