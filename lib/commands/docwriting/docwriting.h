#pragma once
#include <commands/argument.h>
#include <commands/command.h>

namespace cli::commands::docwriting
{
    class DocsNotBuildException : public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };
    
    std::string generateOptionsDocString(const PositionalArgument &argument);

    std::string generateArgDocString(const PositionalArgument &argument);

    std::string generateLongDocString(const Command &command);

    std::string generateShortDocString(const Command &command);

} // namespace cli::commands