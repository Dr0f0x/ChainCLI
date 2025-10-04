#include "parser.h"
#include <iostream>
#include <ranges>
#include <algorithm>
#include <any>
#include "parser_utils.h"
#include "cli_base.h"

namespace cli::parsing
{

    void cli::parsing::StringParser::parseArguments(
        const cli::commands::Command &command,
        const std::vector<std::string> &inputs,
        ContextBuilder &contextBuilder)
    {
        const auto &posArguments = command.getPositionalArguments();
        const auto &optArguments = command.getOptionArguments();
        const auto &flagArguments = command.getFlagArguments();

        size_t posArgsIndex = 0;
        for (size_t i = 0; i < inputs.size(); ++i)
        {
            const auto &input = inputs[i];

            const cli::commands::OptionArgumentBase *matchedOpt = nullptr;
            for (const auto &opt : optArguments)
            {
                if (input == opt->getShortName() || input == opt->getName())
                {
                    matchedOpt = opt.get();
                    break;
                }
            }
            if (matchedOpt)
            {
                if (matchedOpt->isRepeatable())
                {
                    parseRepeatable(*matchedOpt, inputs[i + 1], contextBuilder);
                }
                else
                {
                    auto val = matchedOpt->parseToValue(inputs[i + 1]);
                    contextBuilder.addOptionArgument(matchedOpt->getName(), val);
                    contextBuilder.addOptionArgument(matchedOpt->getShortName(), val);
                }
                i++;
                continue;
            }

            const cli::commands::FlagArgument *matchedFlag = nullptr;
            for (const auto &flag : flagArguments)
            {
                if (input == flag->getShortName() || input == flag->getName())
                {
                    matchedFlag = flag.get();
                    break;
                }
            }
            if (matchedFlag)
            {
                contextBuilder.addFlagArgument(matchedFlag->getShortName());
                contextBuilder.addFlagArgument(matchedFlag->getName());
                continue;
            }

            if (posArgsIndex >= posArguments.size())
            {
                throw ParseException("More positional arguments provided than command accepts");
            }

            const auto &posArg = *posArguments.at(posArgsIndex);

            if (posArg.isRepeatable())
            {
                parseRepeatable(posArg, input, contextBuilder);
            }
            else
            {
                auto val = posArg.parseToValue(input);
                contextBuilder.addPositionalArgument(posArg.getName(), val);
            }

            ++posArgsIndex;
        }
        checkGroups(command, contextBuilder);
    }

    std::any StringParser::parseRepeatableList(const cli::commands::TypedArgumentBase &arg, const std::string &input)
    {
        std::stringstream ss(input);
        std::string token;
        std::vector<std::any> parsedValues;

        while (std::getline(ss, token, ','))
        {
            // Trim leading whitespace
            token.erase(token.begin(),
                        std::ranges::find_if(token, [](unsigned char ch)
                                             { return !std::isspace(ch); }));

            // Trim trailing whitespace
            token.erase(
                std::ranges::find_if(token | std::views::reverse,
                                     [](unsigned char ch)
                                     { return !std::isspace(ch); })
                    .base(),
                token.end());

            if (!token.empty())
            {
                parsedValues.push_back(arg.parseToValue(token));
            }
        }

        return parsedValues;
    }

    void StringParser::parseRepeatable(const cli::commands::OptionArgumentBase &arg, const std::string &input, ContextBuilder &contextBuilder)
    {
        auto values = parseRepeatableList(arg, input);
        contextBuilder.addOptionArgument(arg.getShortName(), values);
        contextBuilder.addOptionArgument(arg.getName(), values);
    }

    void StringParser::parseRepeatable(const cli::commands::PositionalArgumentBase &arg, const std::string &input, ContextBuilder &contextBuilder)
    {
        auto values = parseRepeatableList(arg, input);
        contextBuilder.addPositionalArgument(arg.getName(), values);
    }

    void StringParser::checkGroups(const cli::commands::Command &command, const ContextBuilder &contextBuilder)
    {
        for (const auto &argGroup : command.getArgumentGroups())
        {
            if (argGroup->isExclusive())
            {
                const cli::commands::ArgumentBase *firstProvided = nullptr;

                for (const auto &argPtr : argGroup->getArguments())
                {
                    if (!firstProvided && contextBuilder.isArgPresent(std::string(argPtr->getName())))
                    {
                        firstProvided = argPtr.get();
                    }
                    else if (contextBuilder.isArgPresent(std::string(argPtr->getName())))
                    {
                        throw GroupParseException("Two arguments of mutually exclusive group were present");
                    }
                }
            }
            else if (argGroup->isInclusive())
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
                        throw GroupParseException("Not all arguments of mutually inclusive group were present");
                    }
                }
            }
        }
    }
}
