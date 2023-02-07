// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include "types/Config.hpp"
#include "types/Index.hpp"

namespace hdoc::serde {
/// @brief Serialize hdoc's index to a single file in JSON format on the disk
std::string serializeToJSON(const hdoc::types::Index& index, const hdoc::types::Config& cfg);

/// @brief Deserialize hdoc's index in JSON format back into hdoc's internal data structures
/// Returns true if the deserialization succeeded, and false if it didn't.
bool deserializeFromJSON(hdoc::types::Index& index, hdoc::types::Config& cfg);

/// @brief Verify that the user's API key is valid prior to uploading documentation
bool verify();

/// @brief Upload the serialized Index to hdoc.io for hosting
void uploadDocs(const std::string_view data);
} // namespace hdoc::serde
