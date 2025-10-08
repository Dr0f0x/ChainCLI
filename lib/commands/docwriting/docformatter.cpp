// Copyright 2025 Dominik Czekai
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "docformatter.h"

#define inline_t

namespace cli::commands::docwriting
{

inline_t std::pair<std::string, std::string> getPositionalArgumentBrackets(bool required)
{
    if (required)
        return {"<", ">"};
    else
        return {"[<", ">]"};
}

inline_t std::pair<char, char> getOptionArgumentBrackets(bool required)
{
    if (required)
        return {'(', ')'};
    else
        return {'[', ']'};
}

std::string DefaultFlagFormatter::generateArgDocString(
    const FlagArgument &argument, [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getOptionArgumentBrackets(argument.isRequired());
    builder << inBracket << argument.getName() << ',' << argument.getShortName() << outBracket;
    return builder.str();
}

std::string DefaultFlagFormatter::generateOptionsDocString(
    const FlagArgument &argument, [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    builder << argument.getName() << ' ' << argument.getShortName();
    return std::format("{:<{}}{:>{}}", builder.str(), configuration.optionsWidth,
                       argument.getUsageComment(), argument.getUsageComment().size());
}

std::string DefaultOptionFormatter::generateArgDocString(
    const OptionArgumentBase &argument, [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getOptionArgumentBrackets(argument.isRequired());
    builder << inBracket << argument.getName() << ' ' << argument.getShortName() << ' ';
    builder << '<' << argument.getValueName() << '>' << outBracket;

    if (argument.isRepeatable())
        builder << "...";
    return builder.str();
}

std::string DefaultOptionFormatter::generateOptionsDocString(
    const OptionArgumentBase &argument, [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    builder << argument.getName() << ',' << argument.getShortName() << ' ' << '<'
            << argument.getValueName() << '>';
    if (argument.isRepeatable())
        builder << "...";
    return std::format("{:<{}}{:>{}}", builder.str(), configuration.optionsWidth,
                       argument.getUsageComment(), argument.getUsageComment().size());
}

std::string DefaultPositionalFormatter::generateArgDocString(
    const PositionalArgumentBase &argument, [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getPositionalArgumentBrackets(argument.isRequired());

    builder << inBracket << argument.getName();
    builder << outBracket;

    if (argument.isRepeatable())
        builder << "...";
    return builder.str();
}

std::string DefaultPositionalFormatter::generateOptionsDocString(
    const PositionalArgumentBase &argument, [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getPositionalArgumentBrackets(argument.isRequired());

    builder << inBracket << argument.getName() << outBracket;
    if (argument.isRepeatable())
        builder << "...";
    return std::format("{:<{}}{:>{}}", builder.str(), configuration.optionsWidth,
                       argument.getUsageComment(), argument.getUsageComment().size());
}

inline_t void addGroupArgumentDocString(std::ostringstream &builder,
                                        const cli::commands::ArgumentGroup &groupArgs,
                                        const DocWriter &writer)
{
    auto [inBracket, outBracket] = getOptionArgumentBrackets(groupArgs.isRequired());
    if (groupArgs.isExclusive() || groupArgs.isInclusive())
    {
        builder << inBracket;
    }

    auto args = groupArgs.getArguments();
    for (size_t i = 0; i < args.size(); ++i)
    {
        const auto &argPtr = args[i];
        builder << argPtr->getArgDocString(writer);

        if (i < args.size() - 1) // not the last element
        {
            if (groupArgs.isExclusive())
                builder << " | ";
            else
                builder << " ";
        }
    }

    if (groupArgs.isExclusive() || groupArgs.isInclusive())
    {
        builder << outBracket;
    }
}

std::string DefaultCommandFormatter::generateLongDocString(const Command &command,
                                                           std::string_view fullCommandPath,
                                                           const DocWriter &writer,
                                                           [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    builder << fullCommandPath << " ";

    for (const auto &argGroupPtr : command.getArgumentGroups())
    {
        addGroupArgumentDocString(builder, *argGroupPtr, writer);
        builder << ' ';
    }

    builder << "\n\n" << command.getLongDescription() << "\n\nOptions:\n";

    for (const auto &argGroupPtr : command.getArgumentGroups())
    {
        for (const auto &argPtr : argGroupPtr->getArguments())
        {
            builder << argPtr->getOptionsDocString(writer) << "\n";
        }
    }
    return builder.str();
}

std::string DefaultCommandFormatter::generateShortDocString(const Command &command,
                                                            std::string_view fullCommandPath,
                                                            const DocWriter &writer,
                                                            [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    builder << fullCommandPath << " ";

    for (const auto &argGroupPtr : command.getArgumentGroups())
    {
        addGroupArgumentDocString(builder, *argGroupPtr, writer);
        builder << " ";
    }
    builder << "\n" << command.getShortDescription();
    return builder.str();
}

} // namespace cli::commands::docwriting