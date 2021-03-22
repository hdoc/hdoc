#pragma once

#include "types/Config.hpp"
#include "types/Index.hpp"

namespace hdoc::serde {
/// @brief Serialize hdoc's index to a single file in binary format on the disk
std::string serialize(const hdoc::types::Index& index, const hdoc::types::Config& cfg);

/// @brief Deserialize hdoc's index in binary format back to it's normal form
void deserialize(hdoc::types::Index& index, hdoc::types::Config& cfg);

/// @brief Verify that the user's API key is valid prior to uploading documentation
bool verify();

/// @brief Upload the serialized Index to hdoc.io for hosting
void uploadDocs(const std::string& data);
} // namespace hdoc::serde
