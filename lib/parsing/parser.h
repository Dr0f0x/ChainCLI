#pragma once
#include <string>
#include <vector>
#include <memory>
#include "cli_context.h"
#include "commands/command.h"

namespace cli::parsing
{
    class StringParser
    {
    public:

        static void parseArguments(
            const cli::commands::Command& command,
            const std::vector<std::string>& inputs,
            ContextBuilder& contextBuilder);

        static void checkGroups(
            const cli::commands::Command& command,
            const ContextBuilder& contextBuilder);
    };
}