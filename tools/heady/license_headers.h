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