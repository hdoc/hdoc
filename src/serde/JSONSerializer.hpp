// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include "serde/SerdeUtils.hpp"
#include "types/Config.hpp"
#include "types/Index.hpp"

#include "rapidjson/prettywriter.h"

namespace hdoc {
namespace serde {

/// @brief Serialize hdoc's index to JSON files
class JSONSerializer {
public:
  template <typename Writer> void serializeSymbol(const hdoc::types::Symbol& sym, Writer& writer) const {
    writer.String("id");
    writer.Uint64(sym.ID.hashValue);
    writer.String("name");
    writer.String(sym.name);
    writer.String("docComment");
    writer.String(sym.docComment);
    writer.String("briefComment");
    writer.String(sym.briefComment);
    writer.String("file");
    writer.String(sym.file);
    writer.String("line");
    writer.Uint64(sym.line);
    writer.String("parentNamespaceID");
    writer.Uint64(sym.parentNamespaceID.hashValue);
  }

  template <typename Writer> void serializeTypeRef(const hdoc::types::TypeRef& typeRef, Writer& writer) const {
    writer.StartObject();
    writer.String("id");
    writer.Uint64(typeRef.id.hashValue);
    writer.String("name");
    writer.String(typeRef.name);
    writer.EndObject();
  }

  template <typename Writer>
  void serializeTemplateParam(const hdoc::types::TemplateParam& tparam, Writer& writer) const {
    writer.StartObject();

    writer.String("templateType");
    writer.Uint64(static_cast<uint64_t>(tparam.templateType));
    writer.String("name");
    writer.String(tparam.name);
    writer.String("type");
    writer.String(tparam.type);
    writer.String("docComment");
    writer.String(tparam.docComment);
    writer.String("isParameterPack");
    writer.Bool(tparam.isParameterPack);
    writer.String("isTypename");
    writer.Bool(tparam.isTypename);

    writer.EndObject();
  }

  template <typename Writer> void serializeFunction(const hdoc::types::FunctionSymbol& f, Writer& writer) const {
    writer.StartObject();
    this->serializeSymbol(f, writer);

    writer.String("isRecordMember");
    writer.Bool(f.isRecordMember);
    writer.String("isConstexpr");
    writer.Bool(f.isConstexpr);
    writer.String("isConsteval");
    writer.Bool(f.isConsteval);
    writer.String("isInline");
    writer.Bool(f.isInline);
    writer.String("isConst");
    writer.Bool(f.isConst);
    writer.String("isVolatile");
    writer.Bool(f.isVolatile);
    writer.String("isRestrict");
    writer.Bool(f.isRestrict);
    writer.String("isVirtual");
    writer.Bool(f.isVirtual);
    writer.String("isVariadic");
    writer.Bool(f.isVariadic);
    writer.String("isNoExcept");
    writer.Bool(f.isNoExcept);
    writer.String("hasTrailingReturn");
    writer.Bool(f.hasTrailingReturn);
    writer.String("isCtorOrDtor");
    writer.Bool(f.isCtorOrDtor);
    writer.String("nameStart");
    writer.Uint64(f.nameStart);
    writer.String("postTemplate");
    writer.Uint64(f.postTemplate);
    writer.String("access");
    writer.Uint64(f.access);
    writer.String("storageClass");
    writer.Uint64(f.storageClass);
    writer.String("refQualifier");
    writer.Uint64(f.refQualifier);
    writer.String("proto");
    writer.String(f.proto);
    writer.String("returnTypeDocComment");
    writer.String(f.returnTypeDocComment);

    writer.Key("returnType");
    this->serializeTypeRef(f.returnType, writer);

    writer.Key("params");
    writer.StartArray();
    for (const auto& param : f.params) {
      writer.StartObject();
      writer.String("name");
      writer.String(param.name);

      writer.Key("type");
      this->serializeTypeRef(param.type, writer);
      writer.String("docComment");
      writer.String(param.docComment);
      writer.String("defaultValue");
      writer.String(param.defaultValue);
      writer.EndObject();
    }
    writer.EndArray();

    writer.Key("templateParams");
    writer.StartArray();
    for (const auto& tparam : f.templateParams) {
      this->serializeTemplateParam(tparam, writer);
    }
    writer.EndArray();
    writer.EndObject();
  }

  template <typename Writer> void serializeFunctions(Writer& writer) const {
    writer.Key("functions");
    writer.StartArray();
    for (const auto& id : getSortedIDs(map2vec(this->index->functions), this->index->functions)) {
      const auto& f = this->index->functions.entries.at(id);
      this->serializeFunction(f, writer);
    }
    writer.EndArray();
  }

  template <typename Writer> void serializeRecord(const hdoc::types::RecordSymbol& s, Writer& writer) const {
    writer.StartObject();

    this->serializeSymbol(s, writer);

    writer.String("type");
    writer.String(s.type);

    writer.String("proto");
    writer.String(s.proto);

    writer.Key("vars");
    writer.StartArray();
    for (const auto& var : s.vars) {
      writer.StartObject();
      writer.String("isStatic");
      writer.Bool(var.isStatic);
      writer.String("name");
      writer.String(var.name);
      writer.Key("type");
      this->serializeTypeRef(var.type, writer);
      writer.String("defaultValue");
      writer.String(var.defaultValue);
      writer.String("docComment");
      writer.String(var.docComment);
      writer.String("access");
      writer.Uint64(var.access);
      writer.EndObject();
    }
    writer.EndArray();

    writer.Key("methodIDs");
    writer.StartArray();
    for (const auto& id : s.methodIDs) {
      writer.Uint64(id.hashValue);
    }
    writer.EndArray();

    writer.Key("baseRecords");
    writer.StartArray();
    for (const auto& br : s.baseRecords) {
      writer.StartObject();
      writer.String("id");
      writer.Uint64(br.id.hashValue);
      writer.String("access");
      writer.Uint64(br.access);
      writer.String("name");
      writer.String(br.name);
      writer.EndObject();
    }
    writer.EndArray();

    writer.Key("templateParams");
    writer.StartArray();
    for (const auto& tparam : s.templateParams) {
      this->serializeTemplateParam(tparam, writer);
    }
    writer.EndArray();

    writer.EndObject();
  }

  template <typename Writer> void serializeRecords(Writer& writer) const {
    writer.Key("records");
    writer.StartArray();
    for (const auto& id : getSortedIDs(map2vec(this->index->records), this->index->records)) {
      const auto& s = this->index->records.entries.at(id);
      this->serializeRecord(s, writer);
    }
    writer.EndArray();
  }

  template <typename Writer> void serializeNamespace(const hdoc::types::NamespaceSymbol& s, Writer& writer) const {
    writer.StartObject();

    this->serializeSymbol(s, writer);

    writer.Key("records");
    writer.StartArray();
    for (const auto& child : s.records) {
      writer.Uint64(child.hashValue);
    }
    writer.EndArray();

    writer.Key("namespaces");
    writer.StartArray();
    for (const auto& child : s.namespaces) {
      writer.Uint64(child.hashValue);
    }
    writer.EndArray();

    writer.Key("enums");
    writer.StartArray();
    for (const auto& child : s.enums) {
      writer.Uint64(child.hashValue);
    }
    writer.EndArray();

    writer.EndObject();
  }

  template <typename Writer> void serializeNamespaces(Writer& writer) const {
    writer.Key("namespaces");
    writer.StartArray();
    for (const auto& id : getSortedIDs(map2vec(this->index->namespaces), this->index->namespaces)) {
      const auto& s = this->index->namespaces.entries.at(id);
      this->serializeNamespace(s, writer);
    }
    writer.EndArray();
  }

  template <typename Writer> void serializeEnum(const hdoc::types::EnumSymbol& e, Writer& writer) const {
    writer.StartObject();

    this->serializeSymbol(e, writer);

    writer.Key("members");
    writer.StartArray();
    for (const auto& member : e.members) {
      writer.StartObject();
      writer.String("name");
      writer.String(member.name);

      writer.String("value");
      writer.Int64(member.value);

      writer.String("docComment");
      writer.String(member.docComment);
      writer.EndObject();
    }
    writer.EndArray();
    writer.EndObject();
  }

  template <typename Writer> void serializeEnums(Writer& writer) const {
    writer.Key("enums");
    writer.StartArray();
    for (const auto& id : getSortedIDs(map2vec(this->index->enums), this->index->enums)) {
      const auto& e = this->index->enums.entries.at(id);
      this->serializeEnum(e, writer);
    }
    writer.EndArray();
  }

  template <typename Writer>
  void serializeMarkdownFile(Writer& writer, const bool isHomepage, const std::filesystem::path& mdPath) const {
    writer.StartObject();
    writer.String("isHomepage");
    writer.Bool(isHomepage);
    writer.String("filename");
    writer.String(mdPath.filename());

    std::string contents = "";
    slurpFile(mdPath, contents);
    writer.String("contents");
    writer.String(contents);

    writer.EndObject();
  }

  template <typename Writer> void serializeMarkdownFiles(Writer& writer) const {
    if (cfg->homepage.empty() == false) {
      serializeMarkdownFile(writer, true, cfg->homepage);
    }
    for (const auto& md : cfg->mdPaths) {
      serializeMarkdownFile(writer, false, md);
    }
  }

  JSONSerializer(const hdoc::types::Index* index, const hdoc::types::Config* cfg) : index(index), cfg(cfg) {}

  std::string getJSONPayload() const {
    rapidjson::StringBuffer                          buf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);

    writer.StartObject();
    writer.Key("config");
    writer.StartObject();
    writer.String("projectName");
    writer.String(this->cfg->projectName);
    writer.String("timestamp");
    writer.String(this->cfg->timestamp);
    writer.String("hdocVersion");
    writer.String(this->cfg->hdocVersion);
    writer.String("gitRepoURL");
    writer.String(this->cfg->gitRepoURL);
    writer.String("gitDefaultBranch");
    writer.String(this->cfg->gitDefaultBranch);
    writer.String("binaryType");
    writer.Uint64(static_cast<uint64_t>(this->cfg->binaryType));
    writer.EndObject();

    writer.Key("index");
    writer.StartObject();
    this->serializeFunctions(writer);
    this->serializeRecords(writer);
    this->serializeEnums(writer);
    this->serializeNamespaces(writer);
    writer.EndObject();

    writer.Key("markdownFiles");
    writer.StartArray();
    this->serializeMarkdownFiles(writer);
    writer.EndArray();
    writer.EndObject();

    return buf.GetString();
  }

private:
  const hdoc::types::Index*  index;
  const hdoc::types::Config* cfg;
};
} // namespace serde
} // namespace hdoc
