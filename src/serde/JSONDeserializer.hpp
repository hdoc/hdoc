// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include "types/Config.hpp"
#include "types/Index.hpp"
#include "types/SerializedMarkdownFile.hpp"

#include "rapidjson/document.h"

#include <optional>

namespace hdoc {
namespace serde {

/// @brief Deserialize JSON to hdoc's data structures.
class JSONDeserializer {
public:
  /// Parse the hdoc-payload.json file in the current directory into a rapidjson::Document, checking
  /// if the JSON is malformed in the process.
  std::optional<rapidjson::Document> parseJSONToDocument() const;

  /// Validate inputJSON against hdoc's schema, which is bundled with the binary.
  bool validateJSON(const rapidjson::Document& inputJSON) const;

  /// Deserialize inputJSON into hdoc's data structures.
  /// This assumes the document has been validated.
  void deserializeJSONPayload(const rapidjson::Document&                        inputJSON,
                              hdoc::types::Index&                               idx,
                              hdoc::types::Config&                              cfg,
                              std::vector<hdoc::types::SerializedMarkdownFile>& mdFiles) const;
  /// Deserialize a JSON value into a hdoc::types::Symbol
  void                         deserialize(hdoc::types::Symbol& base, const rapidjson::Value& obj) const;
  hdoc::types::FunctionSymbol  deserializeFunctionSymbol(const rapidjson::Value& obj) const;
  hdoc::types::RecordSymbol    deserializeRecordSymbol(const rapidjson::Value& obj) const;
  hdoc::types::EnumSymbol      deserializeEnumSymbol(const rapidjson::Value& obj) const;
  hdoc::types::NamespaceSymbol deserializeNamespaceSymbol(const rapidjson::Value& obj) const;
};
} // namespace serde
} // namespace hdoc
