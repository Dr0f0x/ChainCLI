#include "license_headers.h"
#include "Heady.h"
#include "utils.h"

#include <regex>
#define inline_t

namespace Heady::Detail
{

inline_t void RemoveCopyrightHeaders(const Heady::Params &params,
                                     const std::vector<std::string> &licenseHeaders,
                                     std::string &fileData)
{
    if (!licenseHeaders.empty())
    {
        try
        {
            // Remove each header pattern from fileData
            for (const auto &pattern : licenseHeaders)
            {
                std::string escapedPattern = EscapeRegexSpecialChars(pattern);
                std::regex headerRegex(escapedPattern);
                fileData = std::regex_replace(fileData, headerRegex, "");
            }
        }
        catch (const std::exception &e)
        {
            params.logger.error() << "Error processing license header file: " << e.what() << '\n';
        }
    }
}

inline_t std::vector<std::string> ReadLicenseHeaders(const Params &params)
{
    std::vector<std::string> licenseHeaders;
    if (!params.licenseHeader.empty())
    {
        try {
            // Check if file exists first
            if (!std::filesystem::exists(params.licenseHeader)) {
                params.logger.warning() << "License header file does not exist: " << params.licenseHeader << '\n' << std::flush;
                return licenseHeaders;
            }
            
            std::ifstream headerFile(params.licenseHeader);
            if (!headerFile.is_open()) {
                params.logger.warning() << "Could not open license header file: " << params.licenseHeader << '\n' << std::flush;
                return licenseHeaders;
            }

            std::string currentHeader;
            std::string line;
            bool inHeader = false;

            while (std::getline(headerFile, line)) {
                if (line.empty()) {
                    if (inHeader && !currentHeader.empty()) {
                        // End of current header block
                        licenseHeaders.push_back(currentHeader);
                        currentHeader.clear();
                        inHeader = false;
                    }
                } else {
                    if (!inHeader) {
                        inHeader = true;
                    }
                    if (!currentHeader.empty()) {
                        currentHeader += "\n";
                    }
                    currentHeader += line;
                }
            }

            // Add the last header if file doesn't end with empty line
            if (!currentHeader.empty()) {
                licenseHeaders.push_back(currentHeader);
            }

        }
        catch (const std::exception& e) {
            params.logger.error() << "Error processing license header file: " << e.what() << '\n' << std::flush;
        }
    }
    return licenseHeaders;
}

} // namespace Detail
