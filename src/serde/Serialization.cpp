// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#include "serde/Serialization.hpp"
#include "serde/JSONDeserializer.hpp"
#include "serde/JSONSerializer.hpp"
#include "serde/SerdeUtils.hpp"
#include "types/SerializedMarkdownFile.hpp"
#include "types/Symbols.hpp"

#include "rapidjson/prettywriter.h"
#include "spdlog/spdlog.h"

#include <httplib.h>
#include <string>

#ifdef HDOC_RELEASE_BUILD
constexpr char hdocURL[] = "https://app.hdoc.io";
#else
constexpr char hdocURL[] = "https://staging.hdoc.io";
#endif

namespace hdoc::serde {

std::string serializeToJSON(const hdoc::types::Index& index, const hdoc::types::Config& cfg) {
  hdoc::serde::JSONSerializer jsonSerializer(&index, &cfg);
  std::string                 payload = jsonSerializer.getJSONPayload();
  return payload;
}

bool deserializeFromJSON(hdoc::types::Index& index, hdoc::types::Config& cfg) {
  hdoc::serde::JSONDeserializer      jsonDeserializer;
  std::optional<rapidjson::Document> doc = jsonDeserializer.parseJSONToDocument();
  if (doc.has_value() == false) {
    spdlog::error("Unable to parse JSON document, it is likely missing or not valid JSON. Aborting.");
    return false;
  }

  const bool passedSchemaValidation = jsonDeserializer.validateJSON(*doc);
  if (passedSchemaValidation == false) {
    spdlog::error("JSON schema validation of the input JSON file failed. Aborting.");
    return false;
  }

  std::vector<hdoc::types::SerializedMarkdownFile> serializedFiles;
  jsonDeserializer.deserializeJSONPayload(*doc, index, cfg, serializedFiles);

  if (serializedFiles.size() > 0) {
    std::filesystem::path markdownFilesDir = std::filesystem::path("hdoc-markdown-dump");
    std::filesystem::create_directories(markdownFilesDir);

    // Serialized Markdown files are "recreated" (dumped) to a temporary directory
    // The Config object used by the server is then recreated as a copy of the client's
    // but with the paths readjusted
    for (const auto& f : serializedFiles) {
      std::ofstream(markdownFilesDir / f.filename) << f.contents;
      // The homepage isn't added to mdPaths, we don't want it to appear in the sidebar
      if (f.isHomepage == true) {
        cfg.homepage = std::filesystem::path(markdownFilesDir / f.filename);
        continue;
      }
      cfg.mdPaths.emplace_back(markdownFilesDir / f.filename);
    }
  }
  return true;
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

void uploadDocs(const std::string_view data) {
  spdlog::info("Uploading documentation for hosting.");
  const char* val     = std::getenv("HDOC_PROJECT_API_KEY");
  std::string api_key = val == NULL ? std::string("") : std::string(val);
  if (api_key == "") {
    spdlog::error("No API key was found in the HDOC_PROJECT_API_KEY environment variable. Unable to proceed.");
    return;
  }

  httplib::Client  cli(hdocURL);
  cli.set_compress(true);
  httplib::Headers headers{
      {"Authorization", "Api-Key " + api_key},
      {"Content-Disposition", "inline;filename=hdoc-payload.json"},
      {"X-Schema-Version", "v5"},
  };

  const auto res = cli.Put("/api/upload/", headers, data.data(), data.size(), "application/json");
  if (res == nullptr) {
    spdlog::error("Upload failed, unable to proceed. Check that you're connected to the internet.");
    return;
  }

  if (res->status != 200) {
    spdlog::error("Documentation upload failed (status={}): {}", res->status, res->reason);
  } else {
    // Temporarily set the log level to the info level so that the URL to the documentation is
    // printed to the terminal.
    spdlog::set_level(spdlog::level::info);
    spdlog::info("{}", res->body);
    spdlog::set_level(spdlog::level::warn);
  }
}

} // namespace hdoc::serde
