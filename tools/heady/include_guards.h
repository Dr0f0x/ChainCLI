#pragma once

#include "heady_params.h"
#include "utils.h"
#include <filesystem>
#include <regex>
#include <string>

namespace Heady::Detail
{
void RemoveIncludeGuards(const Params &params, std::string &fileData,
                    const std::filesystem::path &filePath);
} // namespace Heady::Detail
