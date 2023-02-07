// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include <filesystem>
#include <string>

#include "cmark-gfm.h"
#include "ctml.hpp"
#include "spdlog/spdlog.h"

namespace hdoc::utils {

/// Converts a Markdown file to an HTML string using GitHub's fork of CommonMark.
struct MarkdownConverter {
  MarkdownConverter(const std::filesystem::path& mdPath);
  ~MarkdownConverter();

  /// Get the HTML node containing the Markdown contents
  CTML::Node getHTMLNode() const;

  cmark_parser* markdownParser = nullptr;
  cmark_node*   markdownDoc    = nullptr;
  char*         htmlBuf        = nullptr;
  std::string   html           = "";
  bool          initialized    = false;
};
} // namespace hdoc::utils
