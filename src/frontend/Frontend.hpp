// Copyright 2019-2023 hdoc
// SPDX-License-Identifier: AGPL-3.0-only

#pragma once

#include "types/Config.hpp"

namespace hdoc::frontend {
/// @brief Parses the .hdoc.toml configuration file and handles the CLI args
class Frontend {
public:
  Frontend(int argc, char** argv, hdoc::types::Config* cfg);
};
} // namespace hdoc::frontend
