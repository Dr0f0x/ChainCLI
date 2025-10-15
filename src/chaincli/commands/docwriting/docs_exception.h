/*
 * Copyright 2025 Dominik Czekai
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
