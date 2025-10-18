/*
The Heady library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Heady.h for license details.
Copyright (c) 2018 James Boer

Modifications and integration with ChainCLI:
Copyright (c) 2025 Dominik Czekai
*/

#include "heady.h"
#include "include_guards.h"
#include "license_headers.h"
#include "utils.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <regex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace Heady
{
namespace Detail
{
// Forward declaration
void FindAndProcessLocalIncludes(const Params &params,
                                 const std::vector<std::string> &licenseHeaders,
                                 const std::list<std::filesystem::directory_entry> &dirEntries,
                                 const std::filesystem::directory_entry &dirEntry,
                                 std::set<std::string, std::less<>> &processed,
                                 std::set<std::string> &systemIncludes, std::string &outputText,
                                 int depth = 0);

inline_t void FindSystemIncludes(const Params &params,
                                         std::set<std::string> &systemIncludes,
                                         std::string &fileData)
{
    std::regex systemIncludeRegex(R"regex(\s*#\s*include\s*<([^>]+)>)regex");
    std::regex ifdefRegex(R"regex(\s*#\s*if(n?def)?\s+([A-Za-z_][A-Za-z0-9_]*))regex");
    std::regex endifRegex(R"regex(\s*#\s*endif\s*(//.*)?$)regex");
    std::smatch systemMatch;

    std::istringstream stream(fileData);
    std::string line;
    std::string processedData;

    int ifdefDepth = 0;
    // Iterate over all lines in the file
    while (std::getline(stream, line))
    {
        if (std::regex_search(line, ifdefRegex))
        {
            ifdefDepth += 1;
        }
        if(std::regex_search(line, endifRegex))
        {
            ifdefDepth = std::max(0, ifdefDepth - 1);
        }

        if (ifdefDepth == 0 && std::regex_search(line, systemMatch, systemIncludeRegex))
        {
            systemIncludes.insert(systemMatch[1].str()); // Add to global set
            // Skip this line (don't add it to processedData) since we're tracking it separately
        }
        else
        {
            processedData += line + "\n";
        }
    }

    // Update fileData with the processed content (system includes removed)
    fileData = processedData;
}

inline_t void FindAndProcessLocalIncludes(
    const Params &params, const std::vector<std::string> &licenseHeaders,
    const std::list<std::filesystem::directory_entry> &dirEntries, const std::string &include,
    std::set<std::string, std::less<>> &processed, std::set<std::string> &systemIncludes,
    std::string &outputText, int depth = 0)
{
    // Check to see if we've already processed this file
    if (processed.contains(include))
    {
        return;
    }

    // Find the directory entry that matches this include filename, and if found,
    // process it
    auto itr = std::ranges::find_if(dirEntries, [&include](const auto &entry) {
        return EndsWithPath(entry.path().string(), include);
    });
    if (itr != dirEntries.end())
    {
        FindAndProcessLocalIncludes(params, licenseHeaders, dirEntries, *itr, processed,
                                    systemIncludes, outputText, depth);
    }
}

inline_t void FindAndProcessLocalIncludes(
    const Params &params, const std::vector<std::string> &licenseHeaders,
    const std::list<std::filesystem::directory_entry> &dirEntries,
    const std::filesystem::directory_entry &dirEntry, std::set<std::string, std::less<>> &processed,
    std::set<std::string> &systemIncludes, std::string &outputText, int depth)
{
    // Check to see if we've already processed this file
    auto fn = dirEntry.path().filename().string();
    if (processed.contains(fn))
    {
        return;
    }
    // Now mark this file as processed, so we don't add it twice to the combined
    // header
    processed.emplace(fn);

    const std::string indent(depth * 3, ' ');
    std::filesystem::path sourcePath(params.sourceFolder);
    auto relativePath = std::filesystem::relative(dirEntry.path(), sourcePath);

    params.logger.info() << indent << "[Start processing] " << relativePath.string() << '\n';

    // Open and read file from dirEntry
    std::ifstream file(dirEntry.path());
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileData = buffer.str();

    // Mark file beginning
    if (params.includeFileHints)
    {
        outputText += "\n\n// begin --- ";
        outputText += dirEntry.path().filename().string();
        outputText += " --- ";
        outputText += "\n\n";
    }

    Detail::RemoveIncludeGuards(params, fileData, dirEntry.path());
    Detail::FindSystemIncludes(params, systemIncludes, fileData);
    Detail::RemoveCopyrightHeaders(params, licenseHeaders, fileData);

    // Find local includes (quotes) for processing
    std::regex r(R"regex(\s*#\s*include\s*(["])([^"]+)(["]))regex");
    std::smatch m;
    std::string s = fileData;
    while (std::regex_search(s, m, r))
    {
        // Insert text found up to the include match
        if (m.prefix().length())
            outputText += m.prefix().str();

        // Insert the include text into the output stream
        FindAndProcessLocalIncludes(params, licenseHeaders, dirEntries, m[2].str(), processed,
                                    systemIncludes, outputText, depth + 1);

        // Continue processing the rest of the file text
        s = m.suffix().str();
    }

    // Copy remaining file text to output
    params.logger.info() << indent << "[Finish processing] " << relativePath.string() << " - Added "
                         << s.length() << " Characters to output\n";
    outputText += s;
    outputText += "\n";

    // Mark file end
    if (params.includeFileHints)
    {
        outputText += "\n// end --- ";
        outputText += dirEntry.path().filename().string();
        outputText += " --- ";
        outputText += "\n\n";
    }
}
} // namespace Detail

inline_t void ReplaceInlinePlaceHolder(const Params &params, std::string &outputText)
{
    std::string inlineValue = params.inlined;

    // Then remove all instances of #define inline_t
    std::string inlineDef = "#define " + inlineValue;
    Detail::FindAndReplaceAll(outputText, inlineDef, "");

    // Replace all instances of a specified macro with 'inline'
    if (inlineValue.empty())
        inlineValue = "inline_t ";
    if (inlineValue[inlineValue.size() - 1] != ' ')
        inlineValue += " ";
    Detail::FindAndReplaceAll(outputText, inlineValue, "inline ");
}

inline_t void GenerateHeader(const Params &params)
{
    if (params.output.empty())
        throw std::invalid_argument("Requires a valid output argument");

    // Add initial file entries from designated source folder
    std::list<std::filesystem::directory_entry> dirEntries;
    if (params.recursiveScan)
    {
        for (const auto &f : std::filesystem::recursive_directory_iterator(params.sourceFolder))
            if (f.is_regular_file()) // Only include regular files
                dirEntries.emplace_back(f);
    }
    else
    {
        for (const auto &f : std::filesystem::directory_iterator(params.sourceFolder))
            dirEntries.emplace_back(f);
    }

    // Create list of excluded filenames
    auto excludedFilenames = Detail::Tokenize(params.excluded);

    // Remove excluded files from fileEntries
    dirEntries.remove_if([&excludedFilenames](const auto &entry) {
        return std::ranges::find(excludedFilenames, entry.path().filename().string()) !=
               excludedFilenames.end();
    });

    params.logger.info() << "Registered files in source folder '" << params.sourceFolder << "':\n";
    std::filesystem::path sourcePath(params.sourceFolder);
    for (const auto &entry : dirEntries)
    {
        auto relativePath = std::filesystem::relative(entry.path(), sourcePath);
        params.logger.info() << "  " << relativePath.string() << '\n';
    }
    params.logger.info() << "----------------------------------------\n" << std::flush;

    // No need to do anything if we don't have any files to process
    if (dirEntries.empty())
        return;

    // Make sure .cpp files are processed first
    dirEntries.sort([](const auto &left, const auto &right) {
        // We're taking advantage of the fact that cpp < h or hpp or inc.  If we
        // need to add other extensions, we'll have to revisit this.
        return left.path().extension() < right.path().extension();
    });

    std::string outputText;
    std::vector<std::string> licenseHeaders = Detail::ReadLicenseHeaders(params);
    if (!licenseHeaders.empty())
    {
        outputText += licenseHeaders[0] + "\n";
    }

    // Amalgamation-specific define for header
    if (!params.define.empty())
    {
        outputText += "\n// Amalgamation-specific define";
        outputText += "\n#ifndef ";
        outputText += params.define;
        outputText += "\n#define ";
        outputText += params.define;
        outputText += "\n#endif\n";
    }

    auto guardName = Detail::CreateGuardName(std::filesystem::path(params.output));
    if (params.useStandardIncludeGuard)
    {
        outputText += "\n#ifndef " + guardName + "\n#define " + guardName + "\n\n";
    }
    else
    {
        outputText += "\n#pragma once\n\n";
    }

    // Recursively combine all source and headers into a single output string
    std::set<std::string, std::less<>> processed;
    std::set<std::string> allSystemIncludes; // Track all system includes across all files

    std::string accumulatedFiles = "";

    for (const auto &entry : dirEntries)
    {
        Detail::FindAndProcessLocalIncludes(params, licenseHeaders, dirEntries, entry, processed,
                                            allSystemIncludes, accumulatedFiles);
    }

    // Log all collected system includes
    if (!allSystemIncludes.empty())
    {
        params.logger.info() << "All system includes found: ";
        for (const auto &inc : allSystemIncludes)
        {
            params.logger.info() << inc << " ";
            outputText += "#include <" + inc + ">\n";
        }
        params.logger.info() << "\n";
    }

    outputText += accumulatedFiles;
    // Replace inline_t placeholders with inline
    ReplaceInlinePlaceHolder(params, outputText);
    Detail::NormalizeNewlines(outputText);

    if (params.useStandardIncludeGuard)
    {
        outputText += "\n#endif // " + guardName + "\n";
    }

    // Check to see if output folder exists.  If not, create it
    auto outFolder = std::filesystem::path(params.output);
    outFolder.remove_filename();
    if (!std::filesystem::exists(outFolder))
    {
        std::filesystem::create_directory(outFolder);
    }
    else
    {
        // Remove existing file
        if (std::filesystem::exists(params.output))
            std::filesystem::remove(params.output);
    }

    // Write all processed file data to new header file
    std::ofstream outFile;
    outFile.open(params.output, std::ios::out);
    outFile << outputText;
    params.logger.info() << std::flush;
}

} // namespace Heady