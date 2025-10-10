/*
The Heady library is distributed under the MIT License (MIT)
https://opensource.org/licenses/MIT
See LICENSE.TXT or Heady.h for license details.
Copyright (c) 2018 James Boer
*/

#include "Heady.h"

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
void FindAndProcessLocalIncludes(const std::list<std::filesystem::directory_entry> &dirEntries,
                                 const std::filesystem::directory_entry &dirEntry,
                                 std::set<std::string, std::less<>> &processed,
                                 std::string &outputText, int depth = 0);

inline_t std::vector<std::string> Tokenize(const std::string_view source)
{
    if (source.empty())
        return {};

    std::string temp(source);
    std::regex r(R"(\s+)");
    std::sregex_token_iterator first(temp.begin(), temp.end(), r, -1);
    std::sregex_token_iterator last;
    return {first, last};
}

inline_t bool EndsWithPath(std::string_view str, std::string_view suffix)
{
    auto normalize = [](std::string_view s) {
        std::string result(s);
        std::ranges::replace(result, '\\', '/');
        return result;
    };

    std::string normStr = normalize(str);
    std::string normSuffix = normalize(suffix);

    if (normStr.size() < normSuffix.size())
        return false;

    // Check if the end matches
    if (normStr.compare(normStr.size() - normSuffix.size(), normSuffix.size(), normSuffix) != 0)
        return false;

    // Ensure proper boundary (either start of string or '/')
    if (size_t pos = normStr.size() - normSuffix.size(); pos == 0 || normStr[pos - 1] == '/')
        return true;

    return false;
}

inline_t void FindAndReplaceAll(std::string &str, std::string_view search, std::string_view replace)
{
    size_t pos = str.find(search);
    while (pos != std::string::npos)
    {
        str.replace(pos, search.size(), replace);
        pos = str.find(search, pos + search.size());
    }
}

inline_t void FindAndProcessLocalIncludes(
    const std::list<std::filesystem::directory_entry> &dirEntries, const std::string &include,
    std::set<std::string, std::less<>> &processed, std::string &outputText, int depth = 0)
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
        FindAndProcessLocalIncludes(dirEntries, *itr, processed, outputText, depth);
    }
}

inline_t void FindAndProcessLocalIncludes(
    const std::list<std::filesystem::directory_entry> &dirEntries,
    const std::filesystem::directory_entry &dirEntry, std::set<std::string, std::less<>> &processed,
    std::string &outputText, int depth)
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

    std::cout << indent << "[Start processing] " << dirEntry.path().string() << '\n';

    // Open and read file from dirEntry
    std::ifstream file(dirEntry.path());
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string fileData = buffer.str();

    // Mark file beginning
    outputText += "\n\n// begin --- ";
    outputText += dirEntry.path().filename().string();
    outputText += " --- ";
    outputText += "\n\n";

    // Find local includes
    std::regex r(R"regex(\s*#\s*include\s*(["])([^"]+)(["]))regex");
    std::smatch m;
    std::string s = fileData;
    while (std::regex_search(s, m, r))
    {
        // Insert text found up to the include match
        if (m.prefix().length())
            outputText += m.prefix().str();

        // Insert the include text into the output stream
        FindAndProcessLocalIncludes(dirEntries, m[2].str(), processed, outputText, depth + 1);

        // Continue processing the rest of the file text
        s = m.suffix().str();
    }

    // Copy remaining file text to output
    std::cout << indent << "[Finish processing] " << dirEntry.path().string() << " - Added "
              << s.length() << " Characters to output\n";
    outputText += s;

    // Mark file end
    outputText += "\n\n// end --- ";
    outputText += dirEntry.path().filename().string();
    outputText += " --- ";
    outputText += "\n\n";
}
} // namespace Detail

inline_t std::string GetVersionString()
{
    std::stringstream ss;
    ss << MajorVersion << '.' << MinorVersion << '.' << PatchNumber;
    return ss.str();
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
        return std::ranges::find(excludedFilenames, entry.path().filename().string()) != excludedFilenames.end();
    });
    
    std::cout << "Registered files in source folder '" << params.sourceFolder << "':\n";
    for (const auto &entry : dirEntries)
    {
        std::cout << "  " << entry.path().string() << '\n';
    }
    std::cout << "----------------------------------------\n";

    // No need to do anything if we don't have any files to process
    if (dirEntries.empty())
        return;

    // Make sure .cpp files are processed first
    dirEntries.sort([](const auto &left, const auto &right) {
        // We're taking advantage of the fact that cpp < h or hpp or inc.  If we
        // need to add other extensions, we'll have to revisit this.
        return left.path().extension() < right.path().extension();
    });

    // Amalgamation-specific define for header
    std::string outputText;
    if (!params.define.empty())
    {
        outputText += "\n// Amalgamation-specific define";
        outputText += "\n#ifndef ";
        outputText += params.define;
        outputText += "\n#define ";
        outputText += params.define;
        outputText += "\n#endif\n";
    }

    // Recursively combine all source and headers into a single output string
    std::set<std::string, std::less<>> processed;
    for (const auto &entry : dirEntries)
    {
        Detail::FindAndProcessLocalIncludes(dirEntries, entry, processed, outputText);
    }

    // Replace all instances of a specified macro with 'inline'
    std::string inlineValue = params.inlined;
    if (inlineValue.empty())
        inlineValue = "inline_t ";
    if (inlineValue[inlineValue.size() - 1] != ' ')
        inlineValue += " ";
    Detail::FindAndReplaceAll(outputText, inlineValue, "inline ");

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
}

} // namespace Heady