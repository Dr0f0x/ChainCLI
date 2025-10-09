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
#include <string>
#include <string_view>
#include <typeindex>

namespace cli::commands
{

/// @brief Enumeration of argument kinds.
enum class ArgumentKind
{
    Positional,
    Option,
    Flag,
};

namespace docwriting
{
class DocWriter; // Forward declaration
}

/// @brief Base class for command-line arguments.
class ArgumentBase
{
public:
    virtual ~ArgumentBase() = default;

#pragma region Accessors

    /// @brief Get the name of the argument.
    /// @details The name is a unique identifier for the argument within its command.
    /// @return The name of the argument.
    [[nodiscard]] constexpr std::string_view getName() const noexcept { return name; }

    /// @brief Get the option comment for the argument.
    /// @details The option comment is a brief description of the argument's purpose, used in
    /// help messages and documentation.
    /// @return The option comment for the argument.
    [[nodiscard]] constexpr std::string_view getOptionComment() const noexcept
    {
        return optionsComment;
    }

    /// @brief Check if the argument is required.
    /// @return True if the argument is required, false otherwise.
    [[nodiscard]] constexpr bool isRequired() const noexcept { return required; }

    /// @brief Check if the argument is repeatable.
    /// @return True if the argument is repeatable, false otherwise.
    [[nodiscard]] constexpr bool isRepeatable() const noexcept { return repeatable; }

    /// @brief Get the type of the argument.
    /// @return The type of the argument.
    [[nodiscard]] constexpr ArgumentKind getArgType() const { return argType; }

    /// @brief Get the options documentation string for the argument.
    /// @details This string is used in the Options section of the help message for a command, where
    /// each argument is listed with its description.
    /// @param writer The documentation writer to use.
    /// @return The options documentation string for the argument.
    [[nodiscard]] virtual std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const = 0;

    /// @brief Get the argument documentation string for the argument.
    /// @details This string is used in the Usage section of the help message for a command and
    /// displays a textual representation of the arguments attributes (e.g. if it is required or
    /// repeatable, ...).
    /// @param writer The documentation writer to use.
    /// @return The argument documentation string for the argument.
    [[nodiscard]] virtual std::string getArgDocString(
        const docwriting::DocWriter &writer) const = 0;
#pragma endregion Accessors

protected:
    ArgumentBase(std::string_view name, std::string_view optionsComment, ArgumentKind argType,
                 bool repeatable, bool required)
        : name(name), optionsComment(optionsComment), argType(argType), repeatable(repeatable),
          required(required)
    {
    }

    const std::string name;
    std::string optionsComment;
    const ArgumentKind argType;
    bool repeatable{false};
    bool required{true};
};

/// @brief Base class for typed command-line arguments.
class TypedArgumentBase
{
public:
    virtual ~TypedArgumentBase() = default;

    /// @brief Get the type of the argument.
    /// @return The type of the argument.
    [[nodiscard]] std::type_index getType() const { return type; }

    /// @brief Parse the input string to the argument's value type.
    /// @param input The input string to parse.
    /// @return The parsed value as std::any.
    [[nodiscard]] virtual std::any parseToValue(const std::string &input) const = 0;

protected:
    explicit TypedArgumentBase(std::type_index t) : type(t) {}

    const std::type_index type;
};

/// @brief Base class for flag-like command-line arguments.
class FlaggedArgumentBase
{
public:
    /// @brief Get the short name of the argument.
    /// @return The short name of the argument.
    [[nodiscard]] constexpr std::string_view getShortName() const noexcept { return shortName; }

protected:
    explicit FlaggedArgumentBase(std::string_view shortName) : shortName(shortName) {}

    std::string shortName;
};

} // namespace cli::commands