#pragma once
#include <filesystem>
#include <regex>
#include <string>
#include <algorithm>

#define inline_t

namespace Heady::Detail
{
inline std::string CreateGuardName(const std::filesystem::path &filePath)
{
    std::string filename = filePath.stem().string(); // Get filename without extension
    std::transform(filename.begin(), filename.end(), filename.begin(), ::toupper);
    std::replace(filename.begin(), filename.end(), '-', '_');
    std::replace(filename.begin(), filename.end(), ' ', '_');
    return filename + "_H";
}

inline void NormalizeNewlines(std::string &text)
{
    std::regex multipleNewlines(R"(\n{3,})");
    text = std::regex_replace(text, multipleNewlines, "\n\n");
}

inline std::string EscapeRegexSpecialChars(const std::string &str)
{
    std::string escaped;
    for (char c : str)
    {
        switch (c)
        {
        case '^':
        case '$':
        case '(':
        case ')':
        case '[':
        case ']':
        case '{':
        case '}':
        case '*':
        case '+':
        case '?':
        case '.':
        case '\\':
        case '|':
            escaped += '\\';
            escaped += c;
            break;
        default:
            escaped += c;
            break;
        }
    }
    return escaped;
}

inline std::vector<std::string> Tokenize(const std::string_view source)
{
    if (source.empty())
        return {};

    std::string temp(source);
    std::regex r(R"(\s+)");
    std::sregex_token_iterator first(temp.begin(), temp.end(), r, -1);
    std::sregex_token_iterator last;
    return {first, last};
}

inline bool EndsWithPath(std::string_view str, std::string_view suffix)
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

inline void FindAndReplaceAll(std::string &str, std::string_view search, std::string_view replace)
{
    size_t pos = str.find(search);
    while (pos != std::string::npos)
    {
        str.replace(pos, search.size(), replace);
        pos = str.find(search, pos + search.size());
    }
}
} // namespace Heady::Detail