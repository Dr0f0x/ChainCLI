#pragma once
#include <stdexcept>
#include <string>
#include <format>
#include "commands/command.h"

namespace cli::commands::docwriting
{
    /// @brief Exception thrown when documentation strings of a command are not built.
class DocsNotBuildException : public std::runtime_error
{
public:
    /// @brief Construct a DocsNotBuildException with a message and command
    /// @param message The error message
    /// @param command The command whose docs weren't built
    DocsNotBuildException(const std::string &message, const Command &command)
        : std::runtime_error(message), command(command)
    {
    }

    /// @brief Gets the command whose documentation wasn't built
    /// @return reference to the command
    const Command &getCommand() const noexcept { return command; }

private:
    const Command &command;
};
} // namespace cli::commands::docwriting
