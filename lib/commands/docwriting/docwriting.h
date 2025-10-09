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
#include <memory>

#include "cli_config.h"
#include "commands/argument.h"
#include "commands/argument_group.h"
#include "commands/command.h"
#include "commands/docwriting/docformatter.h"
#include "commands/flag_argument.h"
#include "commands/option_argument.h"
#include "commands/positional_argument.h"

namespace cli::commands::docwriting
{

/// @brief Exception thrown when documentation strings of a command are not built.
class DocsNotBuildException : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};

/// @brief Documentation writer for CLI commands. Consists of formatters for commands and arguments.
class DocWriter
{

public:
    /// @brief Construct a new DocWriter object.
    /// @param config The CLI configuration.
    explicit DocWriter(const CliConfig &config) : configuration(config) {}

    /// @brief Set the formatter for option arguments.
    /// @param formatter The formatter to use for option arguments.
    void setOptionFormatter(std::unique_ptr<AbstractArgDocFormatter<OptionArgumentBase>> formatter);

    /// @brief Set the formatter for positional arguments.
    /// @param formatter The formatter to use for positional arguments.
    void setPositionalFormatter(
        std::unique_ptr<AbstractArgDocFormatter<PositionalArgumentBase>> formatter);

    /// @brief Set the formatter for flag arguments.
    /// @param formatter The formatter to use for flag arguments.
    void setFlagFormatter(std::unique_ptr<AbstractArgDocFormatter<FlagArgument>> formatter);

    /// @brief Set the formatter for command arguments.
    /// @param formatter The formatter to use for command arguments.
    void setCommandFormatter(std::unique_ptr<AbstractCommandFormatter> formatter);

    /// @brief Build and set the documentation strings for a command.
    /// @param command The command to set the documentation strings for.
    /// @param fullCommandPath The full path of the command.
    void setDocStrings(Command &command, std::string_view fullCommandPath) const;

    /// @brief Generate the long documentation string for a command.
    /// @param command The command to generate the documentation string for.
    /// @param fullCommandPath The full path of the command.
    /// @return The long documentation string for the command.
    std::string generateLongDocString(const Command &command,
                                      std::string_view fullCommandPath) const;

    /// @brief Generate the short documentation string for a command.
    /// @param command The command to generate the documentation string for.
    /// @param fullCommandPath The full path of the command.
    /// @return The short documentation string for the command.
    std::string generateShortDocString(const Command &command,
                                       std::string_view fullCommandPath) const;

    /// @brief Generate the documentation string for a flag argument.
    /// @param argument The flag argument to generate the documentation string for.
    /// @return The documentation string for the flag argument.
    std::string generateOptionsDocString(const FlagArgument &argument) const;

    /// @brief Generate the argument documentation string for a flag argument.
    /// @param argument The flag argument to generate the documentation string for.
    /// @return The argument documentation string for the flag argument.
    std::string generateArgDocString(const FlagArgument &argument) const;

    /// @brief Generate the documentation string for an option argument.
    /// @param argument The option argument to generate the documentation string for.
    /// @return The documentation string for the option argument.
    std::string generateOptionsDocString(const OptionArgumentBase &argument) const;

    /// @brief Generate the argument documentation string for an option argument.
    /// @param argument The option argument to generate the documentation string for.
    /// @return The argument documentation string for the option argument.
    std::string generateArgDocString(const OptionArgumentBase &argument) const;

    /// @brief Generate the documentation string for a positional argument.
    /// @param argument The positional argument to generate the documentation string for.
    /// @return The documentation string for the positional argument.
    std::string generateOptionsDocString(const PositionalArgumentBase &argument) const;

    /// @brief Generate the argument documentation string for a positional argument.
    /// @param argument The positional argument to generate the documentation string for.
    /// @return The argument documentation string for the positional argument.
    std::string generateArgDocString(const PositionalArgumentBase &argument) const;

private:
    const CliConfig &configuration;

    std::unique_ptr<AbstractCommandFormatter> commandFormatterPtr = std::make_unique<DefaultCommandFormatter>();
    std::unique_ptr<AbstractArgDocFormatter<FlagArgument>> flagFormatterPtr  = std::make_unique<DefaultFlagFormatter>();
    std::unique_ptr<AbstractArgDocFormatter<OptionArgumentBase>> optionFormatterPtr  = std::make_unique<DefaultOptionFormatter>();
    std::unique_ptr<AbstractArgDocFormatter<PositionalArgumentBase>> positionalFormatterPtr = std::make_unique<DefaultPositionalFormatter>();
};

} // namespace cli::commands::docwriting