// Copyright 2019-2021 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "serde/Serialization.hpp"
#include "types/Symbols.hpp"

#include "spdlog/spdlog.h"

#include <cereal/archives/portable_binary.hpp>
#include <cereal/types/concepts/pair_associative_container.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cstdlib>
#include <fstream>
#include <httplib.h>
#include <sstream>
#include <streambuf>
#include <string>

#ifdef HDOC_RELEASE_BUILD
constexpr char hdocURL[] = "https://app.hdoc.io";
#else
constexpr char hdocURL[] = "https://staging.hdoc.io";
#endif

namespace hdoc::types {
template <class Archive> static void serialize(Archive& archive, hdoc::types::SymbolID& s) {
  archive(s.hashValue);
}

template <class Archive> static void serialize(Archive& archive, hdoc::types::Symbol& s) {
  archive(s.name, s.briefComment, s.docComment, s.ID, s.file, s.line, s.parentNamespaceID);
}

template <class Archive> static void serialize(Archive& archive, hdoc::types::MemberVariable& s) {
  archive(s.isStatic, s.name, s.type, s.defaultValue, s.docComment, s.access);
}

template <class Archive> static void serialize(Archive& archive, hdoc::types::RecordSymbol::BaseRecord& s) {
  archive(s.id, s.access, s.name);
}

template <class Archive> static void serialize(Archive& archive, hdoc::types::RecordSymbol& s) {
  archive(cereal::base_class<hdoc::types::Symbol>(&s), s.type, s.proto, s.vars, s.methodIDs, s.baseRecords);
}

template <class Archive> static void serialize(Archive& archive, hdoc::types::FunctionParam& s) {
  archive(s.name, s.type, s.defaultValue, s.docComment);
}

template <class Archive> static void serialize(Archive& archive, hdoc::types::TypeRef& s) {
  archive(s.name, s.id);
}

template <class Archive> static void serialize(Archive& archive, hdoc::types::FunctionSymbol& s) {
  archive(cereal::base_class<hdoc::types::Symbol>(&s),
          s.isRecordMember,
          s.isConstexpr,
          s.isConsteval,
          s.isInline,
          s.isConst,
          s.isVolatile,
          s.isRestrict,
          s.isVirtual,
          s.isVariadic,
          s.isNoExcept,
          s.hasTrailingReturn,
          s.isCtorOrDtor,
          s.nameStart,
          s.access,
          s.storageClass,
          s.refQualifier,
          s.proto,
          s.returnType,
          s.returnTypeDocComment,
          s.params);
}

template <class Archive> static void serialize(Archive& archive, hdoc::types::EnumMember& s) {
  archive(s.value, s.name, s.docComment);
}

template <class Archive> static void serialize(Archive& archive, hdoc::types::EnumSymbol& s) {
  archive(cereal::base_class<hdoc::types::Symbol>(&s), s.type, s.members);
}

template <class Archive> static void serialize(Archive& archive, hdoc::types::NamespaceSymbol& s) {
  archive(cereal::base_class<hdoc::types::Symbol>(&s), s.records, s.namespaces, s.enums);
}

template <class Archive> static void serialize(Archive& archive, hdoc::types::Config& s) {
  archive(s.projectName, s.projectVersion, s.timestamp, s.hdocVersion, s.gitRepoURL, s.binaryType);
}

template <class Archive, typename T> static void serialize(Archive& archive, hdoc::types::Database<T>& s) {
  archive(s.entries);
}

template <class Archive> static void serialize(Archive& archive, hdoc::types::Index& s) {
  archive(s.functions, s.records, s.enums, s.namespaces);
}

/// A Markdown file that has been serialized.
/// We need to slurp files from the client's filesystem and
/// later restore them on the server for attached Markdown documentation.
/// This struct provides an interface for that.
struct serializedMdFile {
  bool        isHomepage = false;
  std::string filename;
  std::string contents;
};

template <class Archive> static void serialize(Archive& archive, hdoc::types::serializedMdFile& s) {
  archive(s.isHomepage, s.filename, s.contents);
}
} // namespace hdoc::types

namespace hdoc::serde {

/// Slurp a file into memory
static void slurpFile(const std::filesystem::path& path, std::string& str) {
  std::ifstream t(path);

  // Reserve space in the string to avoid reallocations during slurping
  t.seekg(0, std::ios::end);
  str.reserve(t.tellg());
  t.seekg(0, std::ios::beg);

  str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
}

std::string serialize(const hdoc::types::Index& index, const hdoc::types::Config& cfg) {
  std::stringstream ss;
  {
    cereal::PortableBinaryOutputArchive archive(ss);

    std::vector<hdoc::types::serializedMdFile> serializedFiles;
    serializedFiles.reserve(cfg.homepage.empty() ? cfg.mdPaths.size() : cfg.mdPaths.size() + 1);

    // Serialize all of the client's Markdown files
    // Homepage is a special case and is done separately
    if (cfg.homepage.empty() == false) {
      hdoc::types::serializedMdFile f = {true, cfg.homepage.filename().string(), ""};
      slurpFile(cfg.homepage, f.contents);
      serializedFiles.push_back(f);
    }
    for (const auto& md : cfg.mdPaths) {
      hdoc::types::serializedMdFile f = {false, md.filename().string(), ""};
      slurpFile(md, f.contents);
      serializedFiles.push_back(f);
    }

    archive(index, cfg, serializedFiles);
  }

  return ss.str();
}

void deserialize(hdoc::types::Index& index, hdoc::types::Config& cfg) {
  // Unarchive serialized file from disk
  // The actual work has to happen after destruction of archive
  // because cereal only guarantees everything is done then
  std::vector<hdoc::types::serializedMdFile> serializedFiles;
  {
    std::ifstream                      indexArchive("docs.archive", std::ios::binary);
    cereal::PortableBinaryInputArchive archive(indexArchive);
    archive(index, cfg, serializedFiles);
  }

  if (serializedFiles.size() > 0) {
    // A temporary directory for the markdown files to be dumped into
    // This is some awful code, but it gets the job done and I have other things to deal with
    // TODO: improve this someday
    std::filesystem::path tmpDir = std::filesystem::temp_directory_path() / "hdoc-markdown-dump";
    std::filesystem::create_directories(tmpDir);
    for (uint32_t i = 0; i < UINT32_MAX; i++) {
      const auto path = tmpDir / ("dump" + std::to_string(i)).c_str();
      if (std::filesystem::exists(path) == false) {
        std::filesystem::create_directories(path);
        tmpDir = path;
        break;
      }
    }

    // Serialized Markdown files are "recreated" (dumped) to a temporary directory
    // The Config object used by the server is then recreated as a copy of the client's
    // but with the paths readjusted
    for (const auto& f : serializedFiles) {
      std::ofstream(tmpDir / f.filename) << f.contents;
      // The homepage isn't added to mdPaths, we don't want it to appear in the sidebar
      if (f.isHomepage == true) {
        cfg.homepage = std::filesystem::path(tmpDir / f.filename);
        continue;
      }
      cfg.mdPaths.push_back(tmpDir / f.filename);
    }
  }
}

bool verify() {
  const char* val     = std::getenv("HDOC_PROJECT_API_KEY");
  std::string api_key = val == NULL ? std::string("") : std::string(val);
  if (api_key == "") {
    spdlog::error("No API key was found in the HDOC_PROJECT_API_KEY environment variable. Unable to proceed.");
    return false;
  }

  httplib::Client  cli(hdocURL);
  httplib::Headers headers{
      {"Authorization", "Api-Key " + api_key},
  };

  const auto res = cli.Get("/api/verify/", headers);
  if (res == nullptr) {
    spdlog::error("Connection failed, unable to proceed. Check that you're connected to the internet.");
    return false;
  }

  if (res->status != 200) {
    spdlog::error("Verification failed, ensure your API key is correct and you are subscribed (status={}): {}",
                  res->status,
                  res->reason);
    return false;
  }
  return true;
}

void uploadDocs(const std::string& data) {
  spdlog::info("Uploading documentation for hosting.");
  const char* val     = std::getenv("HDOC_PROJECT_API_KEY");
  std::string api_key = val == NULL ? std::string("") : std::string(val);
  if (api_key == "") {
    spdlog::error("No API key was found in the HDOC_PROJECT_API_KEY environment variable. Unable to proceed.");
    return;
  }

  httplib::Client  cli(hdocURL);
  httplib::Headers headers{
      {"Authorization", "Api-Key " + api_key},
      {"Content-Disposition", "inline;filename=docs.archive"},
      {"X-Schema-Version", "v3"},
  };

  const auto res = cli.Put("/api/upload/", headers, data.data(), data.size(), "application/octet-stream");
  if (res == nullptr) {
    spdlog::error("Upload failed, unable to proceed. Check that you're connected to the internet.");
    return;
  }

  if (res->status != 200) {
    spdlog::error("Documentation upload failed (status={}): {}", res->status, res->reason);
  } else {
    spdlog::info("{}", res->body);
  }
}

} // namespace hdoc::serde
