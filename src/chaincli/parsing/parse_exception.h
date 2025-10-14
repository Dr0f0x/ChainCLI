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
#include <typeinfo>
#include <format>
#include "commands/argument.h"
#include "commands/argument_group.h"

namespace cli::parsing
{
/// @brief Exception thrown when parsing for an argument fails.
class ParseException : public std::runtime_error
{
public:
    /// @brief Construct a ParseException with a message, input string, and argument
    /// @param message The error message
    /// @param input The input string that failed to parse
    /// @param argument The argument that failed to parse
    ParseException(const std::string &message, const std::string &input, const cli::commands::ArgumentBase &argument)
        : std::runtime_error(message), input(input), argument(argument)
    {
    }

    /// @brief Construct a ParseException with default message, input string, and argument
    /// @param input The input string that failed to parse
    /// @param argument The argument that failed to parse
    ParseException(const std::string &input, const cli::commands::ArgumentBase &argument)
        : ParseException(std::format("Failed to parse input '{}' for argument: {}", input, argument.getName()), input, argument)
    {
    }

    /// @brief Gets the input string that failed to parse
    /// @return the input string
    const std::string &getInput() const noexcept { return input; }

    /// @brief Gets the argument that failed to parse
    /// @return reference to the argument
    const cli::commands::ArgumentBase &getArgument() const noexcept { return argument; }

private:
    std::string input;
    const cli::commands::ArgumentBase &argument;
};

/// @brief Exception thrown when the input string cannot be parsed to the needed type for an argument.
class TypeParseException : public std::runtime_error
{
public:
    /// @brief Construct a TypeParseException with a message, input string, and target type
    /// @param message The error message
    /// @param input The input string that couldn't be parsed
    /// @param targetType The type it couldn't be parsed to
    TypeParseException(const std::string &message, const std::string &input, const std::type_info &targetType)
        : std::runtime_error(message), input(input), targetType(targetType)
    {
    }

    /// @brief Construct a TypeParseException with default message, input string, and target type
    /// @param input The input string that couldn't be parsed
    /// @param targetType The type it couldn't be parsed to
    TypeParseException(const std::string &input, const std::type_info &targetType)
        : TypeParseException(std::format("Could not parse '{}' to type '{}'", input, targetType.name()), input, targetType)
    {
    }

    /// @brief Gets the input string that failed to parse
    /// @return the input string
    const std::string &getInput() const noexcept { return input; }

    /// @brief Gets the target type that the input couldn't be parsed to
    /// @return reference to the target type info
    const std::type_info &getTargetType() const noexcept { return targetType; }

private:
    std::string input;
    const std::type_info &targetType;
};

/// @brief Exception thrown when parsing a group of arguments fails.
class GroupParseException : public std::runtime_error
{
public:
    /// @brief Construct a GroupParseException with a message and argument group
    /// @param message The error message
    /// @param argumentGroup The argument group that failed to parse
    GroupParseException(const std::string &message, const cli::commands::ArgumentGroup &argumentGroup)
        : std::runtime_error(message), argumentGroup(argumentGroup)
    {
    }

    /// @brief Construct a GroupParseException with default message and argument group
    /// @param argumentGroup The argument group that failed to parse
    explicit GroupParseException(const cli::commands::ArgumentGroup &argumentGroup)
        : GroupParseException(std::format("Failed to parse argument group"), argumentGroup)
    {
    }

    /// @brief Gets the argument group that failed to parse
    /// @return reference to the argument group
    const cli::commands::ArgumentGroup &getArgumentGroup() const noexcept { return argumentGroup; }

private:
    const cli::commands::ArgumentGroup &argumentGroup;
};

} // namespace cli::parsing