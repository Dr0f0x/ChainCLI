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

        static std::any parseRepeatableList(
            const cli::commands::TypedArgumentBase& arg,
            const std::string& input);

        static void parseRepeatable(
            const cli::commands::OptionArgumentBase& arg,
            const std::string& input,
            ContextBuilder& ContextBuilder);

        static void parseRepeatable(
            const cli::commands::PositionalArgumentBase& arg,
            const std::string& input,
            ContextBuilder& ContextBuilder);

        static void checkGroups(
            const cli::commands::Command& command,
            const ContextBuilder& contextBuilder);
    };
}