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
#include <typeindex>

#include "argument.h"
#include "parsing/parser_utils.h"

namespace cli::commands
{

/// @brief Represents a flag argument in the CLI.
class FlagArgument : public ArgumentBase, public FlaggedArgumentBase
{
public:
    /// @brief Construct a new Flag Argument object.
    /// @param name The name of the argument (usually starts with "--").
    /// @param shortName The short name of the argument (usually starts with "-").
    /// @param optionsComment A comment describing this argument.
    /// @param isRequired Whether the argument is required.
    explicit FlagArgument(std::string_view name, std::string_view shortName = "",
                          std::string_view optionsComment = "", bool isRequired = false)
        : ArgumentBase(name, optionsComment, ArgumentKind::Flag, false, isRequired),
          FlaggedArgumentBase(shortName)
    {
    }

    [[nodiscard]] std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const override;
    [[nodiscard]] std::string getArgDocString(const docwriting::DocWriter &writer) const override;

#pragma region ChainingMethods
    /// @brief Set the options comment for the argument.
    /// @details The options comment is a brief description of the argument's purpose, used in
    /// help messages and documentation.
    /// @param comment The options comment to set.
    /// @return A reference to this argument.
    FlagArgument &withOptionsComment(std::string_view comment)
    {
        optionsComment = comment;
        return *this;
    }

    /// @brief Set whether the argument is required.
    /// @param req Whether the argument should be required.
    /// @return A reference to this argument.
    FlagArgument &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    /// @brief Set the short name for the argument.
    /// @details The short name is a single-character alias for the argument, usually prefixed with
    /// a single dash (e.g., "-h" for "--help").
    /// @param name The short name to set.
    /// @return A reference to this argument.
    FlagArgument &withShortName(std::string_view name)
    {
        this->shortName = name;
        return *this;
    }
#pragma endregion ChainingMethods
};

} // namespace cli::commands