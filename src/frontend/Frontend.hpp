#pragma once

#include "types/Config.hpp"

namespace hdoc::frontend {
/// @brief Parses the .hdoc.toml configuration file and handles the CLI args
class Frontend {
public:
  Frontend(int argc, char** argv, hdoc::types::Config* cfg);
};
} // namespace hdoc::frontend
