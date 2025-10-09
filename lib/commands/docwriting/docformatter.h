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
#include <format>
#include <sstream>

#include "cli_config.h"
#include "commands/command.h"
#include "commands/flag_argument.h"
#include "commands/option_argument.h"
#include "commands/positional_argument.h"

namespace cli::commands::docwriting
{

/// @brief Abstract base class for argument documentation formatters.
/// @tparam T The type of argument to format.
template <typename T> class AbstractArgDocFormatter
{
public:
    virtual ~AbstractArgDocFormatter() = default;

    /// @brief Generate the argument documentation string.
    /// @details the argument doc string is the textual representation of the argument and its attributes
    /// @param argument The argument to document.
    /// @param configuration The CLI configuration.
    /// @return The generated documentation string.
    virtual std::string generateArgDocString(const T &argument,
                                             const cli::CliConfig &configuration) = 0;

    /// @brief Generate the options documentation string.
    /// @details the options doc string is used in the options section of the help message
    /// @param argument The argument to document.
    /// @param configuration The CLI configuration.
    /// @return The generated documentation string.
    virtual std::string generateOptionsDocString(const T &argument,
                                                 const cli::CliConfig &configuration) = 0;
};

/// @brief Default formatter for flag arguments.
class DefaultFlagFormatter : public AbstractArgDocFormatter<FlagArgument>
{
public:
    std::string generateArgDocString(const FlagArgument &argument,
                                     const cli::CliConfig &configuration) override;
    std::string generateOptionsDocString(const FlagArgument &argument,
                                         const cli::CliConfig &configuration) override;
};

/// @brief Default formatter for option arguments.
class DefaultOptionFormatter : public AbstractArgDocFormatter<OptionArgumentBase>
{
public:
    std::string generateArgDocString(const OptionArgumentBase &argument,
                                     const cli::CliConfig &configuration) override;
    std::string generateOptionsDocString(const OptionArgumentBase &argument,
                                         const cli::CliConfig &configuration) override;
};

/// @brief Default formatter for positional arguments.
class DefaultPositionalFormatter : public AbstractArgDocFormatter<PositionalArgumentBase>
{
public:
    std::string generateArgDocString(const PositionalArgumentBase &argument,
                                     const cli::CliConfig &configuration) override;
    std::string generateOptionsDocString(const PositionalArgumentBase &argument,
                                         const cli::CliConfig &configuration) override;
};

/// @brief Abstract base class for command documentation formatters.
class AbstractCommandFormatter
{
public:
    virtual ~AbstractCommandFormatter() = default;

    /// @brief Generate the command documentation string.
    /// @param command The command to document.
    /// @param fullCommandPath The full path of the command.
    /// @param writer The documentation writer.
    /// @param configuration The CLI configuration.
    /// @return The generated documentation string.
    virtual std::string generateLongDocString(const Command &command,
                                              std::string_view fullCommandPath,
                                              const DocWriter &writer,
                                              const cli::CliConfig &configuration) = 0;

    /// @brief Generate the short documentation string.
    /// @param command The command to document.
    /// @param fullCommandPath The full path of the command.
    /// @param writer The documentation writer.
    /// @param configuration The CLI configuration.
    /// @return The generated documentation string.
    virtual std::string generateShortDocString(const Command &command,
                                               std::string_view fullCommandPath,
                                               const DocWriter &writer,
                                               const cli::CliConfig &configuration) = 0;
};

/// @brief Default formatter for commands.
class DefaultCommandFormatter : public AbstractCommandFormatter
{
public:
    std::string generateLongDocString(const Command &command, std::string_view fullCommandPath,
                                      const DocWriter &writer,
                                      const cli::CliConfig &configuration) override;

    std::string generateShortDocString(const Command &command, std::string_view fullCommandPath,
                                       const DocWriter &writer,
                                       const cli::CliConfig &configuration) override;
};

} // namespace cli::commands::docwriting