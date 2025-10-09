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
// untemplated base class for storing in same STL

/// @brief Untemplated Base class for positional arguments in the CLI. Used to store all positional arguments
/// in a single container.
class PositionalArgumentBase : public TypedArgumentBase, public ArgumentBase
{
public:
    /// @brief Construct a new Positional Argument object.
    /// @param name The name of the argument.
    /// @param optionsComment A comment describing this argument.
    /// @param repeatable Whether the argument can be specified multiple times.
    /// @param required Whether the argument is required.
    /// @param t The type of the argument's value.
    PositionalArgumentBase(std::string_view name, std::string_view optionsComment, bool repeatable,
                           bool required, std::type_index t)
        : TypedArgumentBase(t),
          ArgumentBase(name, optionsComment, ArgumentKind::Positional, repeatable, required)
    {
    }

    [[nodiscard]] std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const override;
    [[nodiscard]] std::string getArgDocString(const docwriting::DocWriter &writer) const override;
};

/// @brief Represents positional arguments in the CLI.
/// @tparam T The type of the argument's value.
template <typename T> class PositionalArgument : public PositionalArgumentBase
{

public:
    /// @brief Construct a new Positional Argument object.
    /// @tparam T The type of the argument's value.
    /// @param name The name of the argument.
    /// @param optionsComment A comment describing this argument.
    /// @param required Whether the argument is required.
    /// @param repeatable Whether the argument can be specified multiple times.
    explicit PositionalArgument(std::string_view name, std::string_view optionsComment = "",
                                bool required = true, bool repeatable = false)
        : PositionalArgumentBase(name, optionsComment, repeatable, required, typeid(T))
    {
    }

    [[nodiscard]] std::any parseToValue(const std::string &input) const override;

#pragma region ChainingMethods

    /// @brief Set the options comment for the argument.
    /// @details The options comment is a brief description of the argument's purpose, used in
    /// help messages and documentation.
    /// @param comment The options comment to set.
    /// @return A reference to this argument.
    PositionalArgument<T> &withOptionsComment(std::string_view comment)
    {
        optionsComment = comment;
        return *this;
    }

    /// @brief Set whether the argument is required.
    /// @param req Whether the argument should be required.
    /// @return A reference to this argument.
    PositionalArgument<T> &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    /// @brief Set whether the argument can be specified multiple times.
    /// @param rep Whether the argument can be specified multiple times.
    /// @return A reference to this argument.
    PositionalArgument<T> &withRepeatable(bool rep)
    {
        repeatable = rep;
        return *this;
    }
};

template <typename T>
inline std::any PositionalArgument<T>::parseToValue(const std::string &input) const
{
    return cli::parsing::ParseHelper::parse<T>(input);
}
} // namespace cli::commands