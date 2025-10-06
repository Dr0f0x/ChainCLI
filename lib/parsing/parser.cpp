#include "parser.h"
#include <iostream>
#include <ranges>
#include <algorithm>
#include <any>
#include "parser_utils.h"
#include "cli_base.h"

// used by heady
#define inline_t

namespace cli::parsing
{
    std::any StringParser::parseRepeatableList(const cli::commands::TypedArgumentBase &arg, const std::string &input) const
    {
        std::stringstream ss(input);
        std::string token;
        std::vector<std::any> parsedValues;

        while (std::getline(ss, token, configuration.repeatableDelimiter))
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

    void StringParser::parseRepeatable(const cli::commands::OptionArgumentBase &arg, const std::string &input, ContextBuilder &contextBuilder) const
    {
        auto values = parseRepeatableList(arg, input);
        contextBuilder.addOptionArgument(arg.getShortName(), values);
        contextBuilder.addOptionArgument(arg.getName(), values);
    }

    void StringParser::parseRepeatable(const cli::commands::PositionalArgumentBase &arg, const std::string &input, ContextBuilder &contextBuilder) const
    {
        auto values = parseRepeatableList(arg, input);
        contextBuilder.addPositionalArgument(arg.getName(), values);
    }

    bool StringParser::tryOptionArg(const std::vector<std::shared_ptr<cli::commands::OptionArgumentBase>> &optionArguments,
                                    const std::vector<std::string> &inputs,
                                    const std::string &currentParsing,
                                    int index,
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
                if (!matchedOpt->isRepeatable() && contextBuilder.isArgPresent(std::string(matchedOpt->getName())))
                {
                    throw ParseException("Non Repeatable Argument was repeated");
                }

                auto val = matchedOpt->parseToValue(inputs[index + 1]);
                contextBuilder.addOptionArgument(matchedOpt->getName(), val);
                contextBuilder.addOptionArgument(matchedOpt->getShortName(), val);
            }
            index++;
            return true;
        }
        return false;
    }

    bool StringParser::tryFlagArg(const std::vector<std::shared_ptr<cli::commands::FlagArgument>> &flagArguments,
                                  const std::string &currentParsing,
                                  ContextBuilder &contextBuilder) const
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
                throw ParseException("More positional arguments provided than command accepts");
            }

            if (const auto &posArg = *posArguments.at(posArgsIndex);
                posArg.isRepeatable())
            {
                parseRepeatable(posArg, input, contextBuilder);
            }
            else
            {
                if (!posArg.isRepeatable() && contextBuilder.isArgPresent(std::string(posArg.getName())))
                {
                    throw ParseException("Non Repeatable Argument was repeated");
                }
                auto val = posArg.parseToValue(input);
                contextBuilder.addPositionalArgument(posArg.getName(), val);
            }

            ++posArgsIndex;
        }
        checkGroups(command, contextBuilder);
    }

    inline_t void exclusiveCheck(const commands::ArgumentGroup *argGroup, const ContextBuilder &contextBuilder)
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

    inline_t void inclusiveCheck(const commands::ArgumentGroup *argGroup, const ContextBuilder &contextBuilder)
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

    void StringParser::checkGroups(const cli::commands::Command &command, const ContextBuilder &contextBuilder) const
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
        }
    }
}
