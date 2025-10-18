/*
The Heady library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Heady.h for license details.
Copyright (c) 2018 James Boer

Modifications and integration with ChainCLI:
Copyright (c) 2025 Dominik Czekai
*/

#pragma once

#include <filesystem>
#include <fstream>
#include "heady_params.h"

namespace Heady::Detail
{
void RemoveCopyrightHeaders(const Heady::Params &params,
                                     const std::vector<std::string> &licenseHeaders,
                                     std::string &fileData);

std::vector<std::string> ReadLicenseHeaders(const Params &params);
} // namespace Heady::Detail