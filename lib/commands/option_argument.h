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
#include <ostream>
#include <string>
#include <string_view>
#include <typeindex>

#include "argument.h"
#include "parsing/parser_utils.h"

namespace cli::commands
{

/// @brief Untemplated base class for option arguments in the CLI. Used to store all option arguments
/// in a single container.
class OptionArgumentBase : public TypedArgumentBase, public ArgumentBase, public FlaggedArgumentBase
{
public:
    /// @brief Construct a new Option Argument object.
    /// @param name The name of the argument (usually starts with "--").
    /// @param optionsComment A comment describing this argument.
    /// @param repeatable Whether the argument can be specified multiple times.
    /// @param required Whether the argument is required.
    /// @param t The type of the argument's value.
    /// @param shortName The short name for the argument (usually starts with "-").
    /// @param valueName The name of the value for the argument.
    OptionArgumentBase(std::string_view name, std::string_view optionsComment, bool repeatable,
                       bool required, std::type_index t, std::string_view shortName,
                       std::string_view valueName)
        : TypedArgumentBase(t),
          ArgumentBase(name, optionsComment, ArgumentKind::Option, repeatable, required),
          FlaggedArgumentBase(shortName), valueName(valueName)
    {
    }

    [[nodiscard]] std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const override;
    [[nodiscard]] std::string getArgDocString(const docwriting::DocWriter &writer) const override;
    [[nodiscard]] constexpr std::string_view getValueName() const noexcept { return valueName; }

protected:
    const std::string valueName;
};

/// @brief Represents option arguments in the CLI.
/// @tparam T The type of the argument's value.
template <typename T> class OptionArgument : public OptionArgumentBase
{

public:
    /// @brief Construct a new Option Argument object.
    /// @tparam T The type of the argument's value.
    /// @param name The name of the argument (usually starts with "--").
    /// @param valueName The name of the value for the argument.
    /// @param shortName The short name for the argument (usually starts with "-").
    /// @param optionsComment A comment describing this argument.
    /// @param required Whether the argument is required.
    /// @param repeatable Whether the argument can be specified multiple times.
    explicit OptionArgument(std::string_view name, std::string_view valueName,
                            std::string_view shortName = "", std::string_view optionsComment = "",
                            bool required = false, bool repeatable = false)
        : OptionArgumentBase(name, optionsComment, repeatable, required, typeid(T), shortName,
                             valueName)
    {
    }

    [[nodiscard]] std::any parseToValue(const std::string &input) const override;

#pragma region ChainingMethods

    /// @brief Set the options comment for the argument.
    /// @details The options comment is a brief description of the argument's purpose, used in
    /// help messages and documentation.
    /// @param comment The options comment to set.
    /// @return A reference to this argument.
    OptionArgument<T> &withOptionsComment(std::string_view comment)
    {
        this->optionsComment = comment;
        return *this;
    }

    /// @brief Set whether the argument is required.
    /// @param req Whether the argument should be required.
    /// @return A reference to this argument.
    OptionArgument<T> &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    /// @brief Set the short name for the argument.
    /// @details The short name is a single-character alias for the argument, usually prefixed with
    /// a single dash (e.g., "-h" for "--help").
    /// @param name The short name to set.
    /// @return A reference to this argument.
    OptionArgument<T> &withShortName(std::string_view name)
    {
        this->shortName = name;
        return *this;
    }

    /// @brief Set whether the argument can be specified multiple times.
    /// @param rep Whether the argument can be specified multiple times.
    /// @return A reference to this argument.
    OptionArgument<T> &withRepeatable(bool rep)
    {
        repeatable = rep;
        return *this;
    }
};

template <typename T>
inline std::any OptionArgument<T>::parseToValue(const std::string &input) const
{
    return cli::parsing::ParseHelper::parse<T>(input);
}
} // namespace cli::commands