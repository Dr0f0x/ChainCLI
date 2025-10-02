#pragma once
#include <string>
#include <vector>
#include <memory>
#include "cli_context.h"
#include "commands/positional_argument.h"
#include "commands/option_argument.h"
#include "commands/flag_argument.h"

namespace cli::parsing
{
    class StringParser
    {
    public:

        static void parseArguments(
            const std::vector<std::unique_ptr<cli::commands::PositionalArgumentBase>> &posArguments,
            const std::vector<std::unique_ptr<cli::commands::OptionArgumentBase>> &optArguments,
            const std::vector<std::unique_ptr<cli::commands::FlagArgument>> &flagArguments,
            const std::vector<std::string>& inputs,
            ContextBuilder& contextBuilder);
    };
}