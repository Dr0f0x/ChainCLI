#include "parser.h"
#include <iostream>
#include <any>
#include "parser_utils.h"
#include "cli_base.h"

namespace cli::parsing
{

    void cli::parsing::StringParser::parseArguments(
        const cli::commands::Command& command,
        const std::vector<std::string> &inputs,
        ContextBuilder &contextBuilder)
    {
        const auto &posArguments = command.getPositionalArguments();
        const auto &optArguments = command.getOptionArguments();
        const auto &flagArguments = command.getFlagArguments();

        int posArgsIndex = 0;
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
                //contextBuilder.addOptionArgumentPresence(matchedOpt->getName());
                //contextBuilder.addOptionArgumentPresence(matchedOpt->getShortName());

                continue;
            }

            const auto &posArg = *posArguments.at(posArgsIndex);

            auto val = posArg.parseToValue(input);
            contextBuilder.addPositionalArgument(posArg.getName(), val);
            ++posArgsIndex;
        }
    }
}
