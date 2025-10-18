/*
The Heady library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Heady.h for license details.
Copyright (c) 2018 James Boer

Modifications and integration with ChainCLI:
Copyright (c) 2025 Dominik Czekai
*/

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
