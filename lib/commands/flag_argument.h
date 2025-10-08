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
class FlagArgument : public ArgumentBase, public FlaggedArgumentBase
{
public:
    explicit FlagArgument(std::string_view name, std::string_view shortName = "",
                          std::string_view optionsComment = "", bool isRequired = false)
        : ArgumentBase(name, optionsComment, ArgumentKind::Flag, false, isRequired),
          FlaggedArgumentBase(shortName)
    {
    }

    [[nodiscard]] std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const override;
    [[nodiscard]] std::string getArgDocString(const docwriting::DocWriter &writer) const override;

    FlagArgument &withOptionsComment(std::string_view comment)
    {
        optionsComment = comment;
        return *this;
    }

    FlagArgument &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    FlagArgument &withShortName(std::string_view name)
    {
        this->shortName = name;
        return *this;
    }
};
} // namespace cli::commands