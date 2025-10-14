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
#include <string>
#include <vector>

#include "cli_config.h"
#include "cli_context.h"
#include "commands/command.h"
#include "context_builder.h"

namespace cli::parsing
{
/// @brief Class used to parse string inputs into typed argument values based on the command
/// definition.
class Parser
{
public:
    /// @brief Creates a new Parser instance with the given configuration.
    /// @param config The configuration to use for the parser.
    explicit Parser(const CliConfig &config) : configuration(config) {}

    /// @brief Parse the given inputs according to the specified command and populate the context
    /// builder with the parsed values.
    /// @param command The command to parse the inputs for.
    /// @param inputs The inputs to parse.
    /// @param contextBuilder The context builder to populate with the parsed values.
    void parseArguments(const cli::commands::Command &command,
                        const std::vector<std::string> &inputs,
                        ContextBuilder &contextBuilder) const;

private:
    std::any parseRepeatableList(const cli::commands::TypedArgumentBase &arg,
                                 const std::string &input) const;

    void parseRepeatable(const cli::commands::OptionArgumentBase &arg, const std::string &input,
                         ContextBuilder &ContextBuilder) const;

    void parseRepeatable(const cli::commands::PositionalArgumentBase &arg, const std::string &input,
                         ContextBuilder &ContextBuilder) const;

    void checkGroupsAndRequired(const cli::commands::Command &command,
                     const ContextBuilder &contextBuilder) const;

    bool tryOptionArg(
        const std::vector<std::shared_ptr<cli::commands::OptionArgumentBase>> &optionArguments,
        const std::vector<std::string> &inputs, const std::string &currentParsing, size_t index,
        ContextBuilder &contextBuilder) const;

    bool tryFlagArg(const std::vector<std::shared_ptr<cli::commands::FlagArgument>> &flagArguments,
                    const std::string &currentParsing, ContextBuilder &contextBuilder) const;

    const CliConfig &configuration;
};
} // namespace cli::parsing