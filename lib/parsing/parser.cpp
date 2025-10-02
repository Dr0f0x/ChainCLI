#include "parser.h"
#include <iostream>
#include <any>
#include "parser_utils.h"
#include "cli_base.h"

namespace cli::parsing
{

    void cli::parsing::StringParser::parseArguments(
        const std::vector<std::unique_ptr<cli::commands::PositionalArgumentBase>> &posArguments,
        const std::vector<std::unique_ptr<cli::commands::OptionArgumentBase>> &optArguments,
        const std::vector<std::unique_ptr<cli::commands::FlagArgument>> &flagArguments,
        const std::vector<std::string> &inputs,
        ContextBuilder &contextBuilder)
    {
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

            const auto &posArg = *posArguments[posArgsIndex];

            auto val = posArg.parseToValue(input);
            contextBuilder.addPositionalArgument(posArg.getName(), val);
            ++posArgsIndex;
        }
    }
}
