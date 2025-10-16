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

#include "parser.h"

#include <algorithm>
#include <any>
#include <iostream>
#include <ranges>

#include "cli_app.h"
#include "parser_utils.h"

// used by heady
#define inline_t

namespace cli::parsing
{
std::vector<std::any> Parser::parseRepeatableList(const cli::commands::TypedArgumentBase &arg,
                                     const std::string &input) const
{
    std::stringstream ss(input);
    std::string token;
    std::vector<std::any> parsedValues;

    while (std::getline(ss, token, configuration.repeatableDelimiter))
    {
        // Trim leading whitespace
        token.erase(token.begin(), std::ranges::find_if(
                                       token, [](unsigned char ch) { return !std::isspace(ch); }));

        // Trim trailing whitespace
        token.erase(std::ranges::find_if(token | std::views::reverse,
                                         [](unsigned char ch) { return !std::isspace(ch); })
                        .base(),
                    token.end());

        if (!token.empty())
        {
            parsedValues.push_back(arg.parseToValue(token));
        }
    }

    return parsedValues;
}

void Parser::parseRepeatable(const cli::commands::OptionArgumentBase &arg, const std::string &input,
                             ContextBuilder &contextBuilder) const
{
    auto values = parseRepeatableList(arg, input);
    contextBuilder.addRepeatableOptionArgument(arg.getName(), values);
}

void Parser::parseRepeatable(const cli::commands::PositionalArgumentBase &arg,
                             const std::string &input, ContextBuilder &contextBuilder) const
{
    auto values = parseRepeatableList(arg, input);
    contextBuilder.addRepeatablePositionalArgument(arg.getName(), values);
}

bool Parser::tryOptionArg(
    const std::vector<std::shared_ptr<cli::commands::OptionArgumentBase>> &optionArguments,
    const std::vector<std::string> &inputs, const std::string &currentParsing, size_t index,
    ContextBuilder &contextBuilder) const
{
    const cli::commands::OptionArgumentBase *matchedOpt = nullptr;
    for (const auto &opt : optionArguments)
    {
        if (currentParsing == opt->getShortName() || currentParsing == opt->getName())
        {
            matchedOpt = opt.get();
            break;
        }
    }
    if (matchedOpt)
    {
        if (matchedOpt->isRepeatable())
        {
            parseRepeatable(*matchedOpt, inputs[index + 1], contextBuilder);
        }
        else
        {
            if (!matchedOpt->isRepeatable() &&
                contextBuilder.isArgPresent(std::string(matchedOpt->getName())))
            {
                throw ParseException(std::format("Non Repeatable Argument {} was repeated", matchedOpt->getName()), inputs[index + 1], *matchedOpt);
            }

            auto val = matchedOpt->parseToValue(inputs[index + 1]);
            contextBuilder.addOptionArgument(matchedOpt->getName(), val);
        }
        return true;
    }
    return false;
}

bool Parser::tryFlagArg(
    const std::vector<std::shared_ptr<cli::commands::FlagArgument>> &flagArguments,
    const std::string &currentParsing, ContextBuilder &contextBuilder) const
{
    const cli::commands::FlagArgument *matchedFlag = nullptr;
    for (const auto &flag : flagArguments)
    {
        if (currentParsing == flag->getShortName() || currentParsing == flag->getName())
        {
            matchedFlag = flag.get();
            break;
        }
    }
    if (matchedFlag)
    {
        contextBuilder.addFlagArgument(matchedFlag->getShortName());
        contextBuilder.addFlagArgument(matchedFlag->getName());
        return true;
    }
    return false;
}

void cli::parsing::Parser::parseArguments(const cli::commands::Command &command,
                                          const std::vector<std::string> &inputs,
                                          ContextBuilder &contextBuilder) const
{
    const auto &posArguments = command.getPositionalArguments();
    const auto &optArguments = command.getOptionArguments();
    const auto &flagArguments = command.getFlagArguments();

    size_t posArgsIndex = 0;
    for (size_t i = 0; i < inputs.size(); ++i)
    {
        const auto &input = inputs[i];
        if (tryOptionArg(optArguments, inputs, input, i, contextBuilder))
        {
            i++;
            continue;
        }

        if (tryFlagArg(flagArguments, input, contextBuilder))
        {
            continue;
        }

        if (posArgsIndex >= posArguments.size())
        {
            throw ParseException(std::format("More positional arguments were provided than the command accepts with input: {}", input), input, *(posArguments.back()));
        }

        if (const auto &posArg = *posArguments.at(posArgsIndex); posArg.isRepeatable())
        {
            parseRepeatable(posArg, input, contextBuilder);
        }
        else
        {
            if (!posArg.isRepeatable() &&
                contextBuilder.isArgPresent(std::string(posArg.getName())))
            {
                throw ParseException(std::format("Non Repeatable Argument {} was repeated", posArg.getName()), input, posArg);
            }
            auto val = posArg.parseToValue(input);
            contextBuilder.addPositionalArgument(posArg.getName(), val);
        }

        ++posArgsIndex;
    }
    checkGroupsAndRequired(command, contextBuilder);
}

inline_t void exclusiveCheck(const commands::ArgumentGroup *argGroup,
                             const ContextBuilder &contextBuilder)
{
    const cli::commands::ArgumentBase *firstProvided = nullptr;

    for (const auto &argPtr : argGroup->getArguments())
    {
        if (!firstProvided && contextBuilder.isArgPresent(std::string(argPtr->getName())))
        {
            firstProvided = argPtr.get();
        }
        else if (contextBuilder.isArgPresent(std::string(argPtr->getName())) && firstProvided != nullptr)
        {
            throw GroupParseException(std::format("Two arguments of mutually exclusive group were present: {} and {}", firstProvided->getName(), argPtr->getName()), *argGroup);
        }
    }
}

inline_t void inclusiveCheck(const commands::ArgumentGroup *argGroup,
                             const ContextBuilder &contextBuilder)
{
    const cli::commands::ArgumentBase *firstProvided = nullptr;

    for (const auto &argPtr : argGroup->getArguments())
    {
        if (!firstProvided && contextBuilder.isArgPresent(std::string(argPtr->getName())))
        {
            firstProvided = argPtr.get();
        }
        else if (!contextBuilder.isArgPresent(std::string(argPtr->getName())))
        {
            throw GroupParseException(std::format("Missing argument in mutually exclusive group: {}", argPtr->getName()), *argGroup);
        }
    }
}

inline_t void checkRequired(const commands::ArgumentGroup *argGroup, const ContextBuilder &contextBuilder)
{
    for (const auto &argPtr : argGroup->getArguments())
    {
        if (argPtr->isRequired() && !contextBuilder.isArgPresent(std::string(argPtr->getName())))
        {
            throw ParseException(std::format("Required argument {} is missing", argPtr->getName()),"", *argPtr);
        }
    }
}

void Parser::checkGroupsAndRequired(const cli::commands::Command &command,
                         const ContextBuilder &contextBuilder) const
{
    for (const auto &argGroup : command.getArgumentGroups())
    {
        if (argGroup->isExclusive())
        {
            exclusiveCheck(argGroup.get(), contextBuilder);
        }
        else if (argGroup->isInclusive())
        {
            inclusiveCheck(argGroup.get(), contextBuilder);
        }
        checkRequired(argGroup.get(), contextBuilder);
    }
}

} // namespace cli::parsing
