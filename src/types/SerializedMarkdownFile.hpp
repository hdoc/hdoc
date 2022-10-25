// Copyright 2019-2022 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include <string>

namespace hdoc::types {

/// A Markdown file that has been serialized.
/// We need to slurp files from the client's filesystem and
/// later restore them on the server for attached Markdown documentation.
/// This struct provides an interface for that.
struct SerializedMarkdownFile {
  bool        isHomepage = false;
  std::string filename;
  std::string contents;
};

} // namespace hdoc::types
