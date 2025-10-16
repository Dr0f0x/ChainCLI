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
#include <any>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "cli_context.h"
#include "logging/logger.h"

namespace cli
{

/// @brief Builder for CliContext objects, allowing to incrementally add arguments before
/// constructing the final context object.
class ContextBuilder
{
public:
    /// @brief Constructs a new ContextBuilder instance.
    ContextBuilder();

    /// @brief Add a positional argument to the context being built.
    /// @param argName the name of the positional argument
    /// @param val value of the positional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addPositionalArgument(const std::string &argName, std::any &val);

    /// @brief Add a positional argument to the context being built.
    /// @param argName the name of the positional argument
    /// @param val value of the positional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addPositionalArgument(std::string_view argName, std::any &val);


    /// @brief Add a repeatable positional argument to the context being built.
    /// @param argName the name of the repeatable positional argument
    /// @param vals values of the repeatable positional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addRepeatablePositionalArgument(const std::string &argName, const std::vector<std::any> &vals);

    /// @brief Add a repeatable positional argument to the context being built.
    /// @param argName the name of the repeatable positional argument
    /// @param vals values of the repeatable positional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addRepeatablePositionalArgument(std::string_view argName, const std::vector<std::any> &vals);

    /// @brief Add an optional argument to the context being built.
    /// @param argName the name of the optional argument
    /// @param val  value of the optional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addOptionArgument(const std::string &argName, std::any &val);

    /// @brief Add an optional argument to the context being built.
    /// @param argName the name of the optional argument
    /// @param val  value of the optional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addOptionArgument(std::string_view argName, std::any &val);

    /// @brief Add a repeatable optional argument to the context being built.
    /// @param argName the name of the repeatable optional argument
    /// @param vals values of the repeatable optional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addRepeatableOptionArgument(const std::string &argName, const std::vector<std::any> &vals);

    /// @brief Add a repeatable optional argument to the context being built.
    /// @param argName the name of the repeatable optional argument
    /// @param vals values of the repeatable optional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addRepeatableOptionArgument(std::string_view argName, const std::vector<std::any> &vals);

    /// @brief Add a flag argument to the context being built.
    /// @param argName the name of the flag argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addFlagArgument(const std::string &argName);

    /// @brief Add a flag argument to the context being built
    /// @param argName the name of the flag argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addFlagArgument(std::string_view argName);

    /// @brief Checks if an argument with the given name is present in the context being built.
    /// @param argName the name of the argument to search for
    /// @return true if the argument is present, false otherwise
    bool isArgPresent(const std::string &argName) const;

    /// @brief Builds the final CliContext object from the accumulated arguments.
    /// @param logger the logger instance to use in the created context
    /// @return a unique_ptr to the created CliContext object
    std::unique_ptr<CliContext> build(cli::logging::AbstractLogger &logger);

private:
    std::unique_ptr<std::unordered_map<std::string, std::any>> positionalArgs;
    std::unique_ptr<std::unordered_map<std::string, std::any>> optionalArgs;
    std::unique_ptr<std::unordered_set<std::string>> flagArgs;
};

} // namespace cli