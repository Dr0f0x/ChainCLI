/*
The Heady library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or heady.h for license details.
Copyright (c) 2018 James Boer

Modifications and integration with ChainCLI:
Copyright (c) 2025 Dominik Czekai
*/

#include "include_guards.h"

namespace Heady::Detail
{
inline_t void RemoveIncludeGuards(const Params &params, std::string &fileData,
                                  const std::filesystem::path &filePath)
{
    // Remove #pragma once lines
    std::regex pragmaOnceRegex(R"(.*#pragma\s+once.*\n?)");
    fileData = std::regex_replace(fileData, pragmaOnceRegex, "");

    if (params.useStandardIncludeGuard)
    {
        // Remove traditional include guards using actual filename
        std::string guardName = Detail::CreateGuardName(filePath);

        std::string includeGuardPattern = R"(\s*#\s*ifndef\s+)" + guardName +
                                          R"([^\n]*\n\s*#\s*define\s+)" + guardName + R"([^\n]*\n)";
        std::regex includeGuardRegex(includeGuardPattern);
        fileData = std::regex_replace(fileData, includeGuardRegex, "");

        // Remove #endif only if it's the very last non-whitespace line
        std::regex endifRegex(R"(\n\s*#\s*endif\s*(//.*)?(\s*\n)*$)");
        fileData = std::regex_replace(fileData, endifRegex, "");
    }
}
} // namespace Heady::Detail
