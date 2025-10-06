#pragma once
#include "commands/argument.h"
#include "commands/command.h"
#include "commands/option_argument.h"
#include "commands/positional_argument.h"
#include "commands/flag_argument.h"
#include "commands/argument_group.h"

namespace cli::commands::docwriting
{
    class DocsNotBuildException : public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };

    std::string generateLongDocString(const Command &command, std::string_view fullCommandPath);

    std::string generateShortDocString(const Command &command, std::string_view fullCommandPath);

    std::string generateOptionsDocString(const FlagArgument &argument);

    std::string generateArgDocString(const FlagArgument &argument);

    std::string generateOptionsDocString(const OptionArgumentBase &argument);

    std::string generateArgDocString(const OptionArgumentBase &argument);

    std::string generateOptionsDocString(const PositionalArgumentBase &argument);

    std::string generateArgDocString(const PositionalArgumentBase &argument);

} // namespace cli::commands