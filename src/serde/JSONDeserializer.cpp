// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "serde/JSONDeserializer.hpp"
#include "serde/SerdeUtils.hpp"

#include "rapidjson/schema.h"
#include "rapidjson/stringbuffer.h"
#include "spdlog/spdlog.h"

extern uint8_t ___schemas_hdoc_payload_schema_json[];

namespace hdoc {
namespace serde {

std::optional<rapidjson::Document> JSONDeserializer::parseJSONToDocument() const {
  rapidjson::Document         doc;
  std::string                 jsonContents;
  const std::filesystem::path hdocPayloadPath = std::filesystem::path("hdoc-payload.json");
  if (std::filesystem::exists(hdocPayloadPath) == false) {
    spdlog::error("hdoc-payload.json is missing from the current directory, unable to parse. Aborting.");
    return std::nullopt;
  }
  slurpFile(hdocPayloadPath, jsonContents);

  if (doc.Parse(jsonContents).HasParseError()) {
    spdlog::error("JSON payload has a parse error and is unreadable. Aborting.");
    return std::nullopt;
  }

  return doc;
}

bool JSONDeserializer::validateJSON(const rapidjson::Document& inputJSON) const {
  const std::string   schemaJSON = reinterpret_cast<char*>(___schemas_hdoc_payload_schema_json);
  rapidjson::Document sd;

  if (sd.Parse(schemaJSON).HasParseError()) {
    spdlog::error("JSON schema bundled with hdoc is not valid");
    return false;
  }

  rapidjson::SchemaDocument  schema(sd);
  rapidjson::SchemaValidator validator(schema);
  if (inputJSON.Accept(validator) == false) {
    rapidjson::StringBuffer sb;
    validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
    spdlog::error("Input JSON document failed schema validation. Member {} failed the {} schema requirement. Aborting.",
                  sb.GetString(),
                  validator.GetInvalidSchemaKeyword());
    return false;
  }

  return true;
}

void JSONDeserializer::deserializeJSONPayload(const rapidjson::Document&                        inputJSON,
                                              hdoc::types::Index&                               idx,
                                              hdoc::types::Config&                              cfg,
                                              std::vector<hdoc::types::SerializedMarkdownFile>& mdFiles) const {
  cfg.projectName          = inputJSON["config"]["projectName"].GetString();
  cfg.timestamp            = inputJSON["config"]["timestamp"].GetString();
  cfg.hdocVersion          = inputJSON["config"]["hdocVersion"].GetString();
  cfg.gitRepoURL           = inputJSON["config"]["gitRepoURL"].GetString();
  cfg.gitDefaultBranch     = inputJSON["config"]["gitDefaultBranch"].GetString();
  cfg.binaryType           = static_cast<hdoc::types::BinaryType>(inputJSON["config"]["binaryType"].GetInt64());

  const auto functionsArray = inputJSON["index"]["functions"].GetArray();
  for (auto it = functionsArray.begin(); it != functionsArray.End(); it++) {
    hdoc::types::FunctionSymbol s = this->deserializeFunctionSymbol(*it);
    idx.functions.reserve(s.ID);
    idx.functions.update(s.ID, s);
  }

  const auto recordsArray = inputJSON["index"]["records"].GetArray();
  for (auto it = recordsArray.begin(); it != recordsArray.End(); it++) {
    hdoc::types::RecordSymbol s = this->deserializeRecordSymbol(*it);
    idx.records.reserve(s.ID);
    idx.records.update(s.ID, s);
  }

  const auto enumsArray = inputJSON["index"]["enums"].GetArray();
  for (auto it = enumsArray.begin(); it != enumsArray.End(); it++) {
    hdoc::types::EnumSymbol s = this->deserializeEnumSymbol(*it);
    idx.enums.reserve(s.ID);
    idx.enums.update(s.ID, s);
  }

  const auto namespacesArray = inputJSON["index"]["namespaces"].GetArray();
  for (auto it = namespacesArray.begin(); it != namespacesArray.End(); it++) {
    hdoc::types::NamespaceSymbol s = this->deserializeNamespaceSymbol(*it);
    idx.namespaces.reserve(s.ID);
    idx.namespaces.update(s.ID, s);
  }

  const auto markdownFilesArray = inputJSON["markdownFiles"].GetArray();
  for (auto it = markdownFilesArray.begin(); it != markdownFilesArray.End(); it++) {
    hdoc::types::SerializedMarkdownFile mdFile;
    auto                                json = it->GetObject();
    mdFile.isHomepage                        = json["isHomepage"].GetBool();
    mdFile.contents                          = json["contents"].GetString();
    mdFile.filename                          = json["filename"].GetString();
    mdFiles.emplace_back(mdFile);
  }
}

void JSONDeserializer::deserialize(hdoc::types::Symbol& base, const rapidjson::Value& obj) const {
  base.ID                = hdoc::types::SymbolID(obj["id"].GetUint64());
  base.name              = obj["name"].GetString();
  base.docComment        = obj["docComment"].GetString();
  base.briefComment      = obj["briefComment"].GetString();
  base.file              = obj["file"].GetString();
  base.line              = obj["line"].GetUint64();
  base.parentNamespaceID = hdoc::types::SymbolID(obj["parentNamespaceID"].GetUint64());
}

hdoc::types::FunctionSymbol JSONDeserializer::deserializeFunctionSymbol(const rapidjson::Value& obj) const {
  hdoc::types::FunctionSymbol s;
  deserialize(s, obj);

  s.isRecordMember       = obj["isRecordMember"].GetBool();
  s.isConstexpr          = obj["isConstexpr"].GetBool();
  s.isConsteval          = obj["isConsteval"].GetBool();
  s.isInline             = obj["isInline"].GetBool();
  s.isConst              = obj["isConst"].GetBool();
  s.isVolatile           = obj["isVolatile"].GetBool();
  s.isRestrict           = obj["isRestrict"].GetBool();
  s.isVirtual            = obj["isVirtual"].GetBool();
  s.isVariadic           = obj["isVariadic"].GetBool();
  s.isNoExcept           = obj["isNoExcept"].GetBool();
  s.hasTrailingReturn    = obj["hasTrailingReturn"].GetBool();
  s.isCtorOrDtor         = obj["isCtorOrDtor"].GetBool();
  s.nameStart            = obj["nameStart"].GetUint64();
  s.postTemplate         = obj["postTemplate"].GetUint64();
  s.access               = static_cast<clang::AccessSpecifier>(obj["access"].GetUint64());
  s.storageClass         = static_cast<clang::StorageClass>(obj["storageClass"].GetUint64());
  s.refQualifier         = static_cast<clang::RefQualifierKind>(obj["refQualifier"].GetUint64());
  s.proto                = obj["proto"].GetString();
  s.returnTypeDocComment = obj["returnTypeDocComment"].GetString();

  hdoc::types::TypeRef tr;
  tr.id        = hdoc::types::SymbolID(obj["returnType"].GetObject()["id"].GetUint64());
  tr.name      = obj["returnType"].GetObject()["name"].GetString();
  s.returnType = tr;

  std::vector<hdoc::types::FunctionParam> params;
  const auto                              paramArray = obj["params"].GetArray();
  for (auto it = paramArray.Begin(); it != paramArray.End(); it++) {
    auto                       param = it->GetObject();
    hdoc::types::FunctionParam fp;
    hdoc::types::TypeRef       tr;

    fp.name         = param["name"].GetString();
    fp.docComment   = param["docComment"].GetString();
    fp.defaultValue = param["defaultValue"].GetString();

    tr.id   = hdoc::types::SymbolID(param["type"].GetObject()["id"].GetUint64());
    tr.name = param["type"].GetObject()["name"].GetString();
    fp.type = tr;
    params.emplace_back(fp);
  }
  s.params = params;

  std::vector<hdoc::types::TemplateParam> tparams;
  const auto                              tparamArray = obj["templateParams"].GetArray();
  for (auto it = tparamArray.Begin(); it != tparamArray.End(); it++) {
    auto                       tparam = it->GetObject();
    hdoc::types::TemplateParam tp;

    tp.templateType    = static_cast<hdoc::types::TemplateParam::TemplateType>(tparam["templateType"].GetUint64());
    tp.name            = tparam["name"].GetString();
    tp.type            = tparam["type"].GetString();
    tp.docComment      = tparam["docComment"].GetString();
    tp.isParameterPack = tparam["isParameterPack"].GetBool();
    tp.isTypename      = tparam["isTypename"].GetBool();
    tparams.emplace_back(tp);
  }
  s.templateParams = tparams;

  return s;
}

hdoc::types::RecordSymbol JSONDeserializer::deserializeRecordSymbol(const rapidjson::Value& obj) const {
  hdoc::types::RecordSymbol s;
  deserialize(s, obj);

  s.type  = obj["type"].GetString();
  s.proto = obj["proto"].GetString();

  std::vector<hdoc::types::MemberVariable> vars;
  const auto                               varArray = obj["vars"].GetArray();
  for (auto it = varArray.Begin(); it != varArray.End(); it++) {
    auto                        var = it->GetObject();
    hdoc::types::MemberVariable mv;
    hdoc::types::TypeRef        tr;
    mv.isStatic     = var["isStatic"].GetBool();
    mv.name         = var["name"].GetString();
    mv.docComment   = var["docComment"].GetString();
    mv.defaultValue = var["defaultValue"].GetString();
    mv.access       = static_cast<clang::AccessSpecifier>(var["access"].GetUint64());
    tr.id           = hdoc::types::SymbolID(var["type"].GetObject()["id"].GetUint64());
    tr.name         = var["type"].GetObject()["name"].GetString();
    mv.type         = tr;
    vars.emplace_back(mv);
  }
  s.vars = vars;

  std::vector<hdoc::types::SymbolID> methodIDs;
  const auto                         methodIDsArray = obj["methodIDs"].GetArray();
  for (auto it = methodIDsArray.Begin(); it != methodIDsArray.End(); it++) {
    hdoc::types::SymbolID id = hdoc::types::SymbolID(it->GetUint64());
    methodIDs.emplace_back(id);
  }
  s.methodIDs = methodIDs;

  std::vector<hdoc::types::RecordSymbol::BaseRecord> baseRecords;

  const auto baseRecordsArray = obj["baseRecords"].GetArray();

  for (auto it = baseRecordsArray.Begin(); it != baseRecordsArray.End(); it++) {
    auto                                  baseRecord = it->GetObject();
    hdoc::types::RecordSymbol::BaseRecord br;

    br.id     = hdoc::types::SymbolID(baseRecord["id"].GetUint64());
    br.access = static_cast<clang::AccessSpecifier>(baseRecord["access"].GetUint64());
    br.name   = baseRecord["name"].GetString();

    baseRecords.emplace_back(br);
  }
  s.baseRecords = baseRecords;

  std::vector<hdoc::types::TemplateParam> tparams;

  const auto tparamArray = obj["templateParams"].GetArray();

  for (auto it = tparamArray.Begin(); it != tparamArray.End(); it++) {
    auto                       tparam = it->GetObject();
    hdoc::types::TemplateParam tp;

    tp.templateType    = static_cast<hdoc::types::TemplateParam::TemplateType>(tparam["templateType"].GetUint64());
    tp.name            = tparam["name"].GetString();
    tp.type            = tparam["type"].GetString();
    tp.docComment      = tparam["docComment"].GetString();
    tp.isParameterPack = tparam["isParameterPack"].GetBool();
    tp.isTypename      = tparam["isTypename"].GetBool();

    tparams.emplace_back(tp);
  }
  s.templateParams = tparams;
  return s;
}

hdoc::types::EnumSymbol JSONDeserializer::deserializeEnumSymbol(const rapidjson::Value& obj) const {
  hdoc::types::EnumSymbol s;
  deserialize(s, obj);

  if (obj.HasMember("members")) {
    std::vector<hdoc::types::EnumMember> members;

    const auto memberArray = obj["members"].GetArray();

    for (auto it = memberArray.Begin(); it != memberArray.End(); it++) {
      auto                    member = it->GetObject();
      hdoc::types::EnumMember em;
      hdoc::types::TypeRef    tr;

      em.name       = member["name"].GetString();
      em.value      = member["value"].GetInt64();
      em.docComment = member["docComment"].GetString();

      members.emplace_back(em);
    }
    s.members = members;
  }

  return s;
}

hdoc::types::NamespaceSymbol JSONDeserializer::deserializeNamespaceSymbol(const rapidjson::Value& obj) const {
  hdoc::types::NamespaceSymbol s;
  deserialize(s, obj);

  std::vector<hdoc::types::SymbolID> records;
  const auto                         recordsArray = obj["records"].GetArray();
  for (auto it = recordsArray.Begin(); it != recordsArray.End(); it++) {
    hdoc::types::SymbolID id = hdoc::types::SymbolID(it->GetUint64());
    records.emplace_back(id);
  }
  s.records = records;

  std::vector<hdoc::types::SymbolID> enums;
  const auto                         enumsArray = obj["enums"].GetArray();
  for (auto it = enumsArray.Begin(); it != enumsArray.End(); it++) {
    hdoc::types::SymbolID id = hdoc::types::SymbolID(it->GetUint64());
    enums.emplace_back(id);
  }
  s.enums = enums;

  std::vector<hdoc::types::SymbolID> namespaces;
  const auto                         namespacesArray = obj["namespaces"].GetArray();
  for (auto it = namespacesArray.Begin(); it != namespacesArray.End(); it++) {
    hdoc::types::SymbolID id = hdoc::types::SymbolID(it->GetUint64());
    namespaces.emplace_back(id);
  }
  s.namespaces = namespaces;

  return s;
}

} // namespace serde
} // namespace hdoc
