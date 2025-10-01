#pragma once
#include <string>
#include <vector>
#include <memory>
#include <any>
#include "commands/positional_argument.h"
#include "commands/option_argument.h"
#include "cli_context.h"

namespace cli::parsing
{
    class StringParser
    {
    public:

        static void parseArguments(const std::vector<std::unique_ptr<cli::commands::PositionalArgumentBase>> &posArguments,
            const std::vector<std::unique_ptr<cli::commands::OptionArgument>> &optArguments,
            const std::vector<std::string>& inputs, ContextBuilder& contextBuilder);
    };
}