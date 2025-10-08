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

namespace cli::parsing
{
class StringParser
{
public:
    explicit StringParser(const CliConfig &config) : configuration(config) {}

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

    void checkGroups(const cli::commands::Command &command,
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