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
class OptionArgumentBase : public TypedArgumentBase, public ArgumentBase, public FlaggedArgumentBase
{
public:
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

template <typename T> class OptionArgument : public OptionArgumentBase
{
    friend std::ostream &operator<<(std::ostream &out, const OptionArgument<T> &arg)
    {
        out << arg.name << " (" << arg.optionsComment << ")";
        return out;
    }

public:
    explicit OptionArgument(std::string_view name, std::string_view valueName,
                            std::string_view shortName = "", std::string_view optionsComment = "",
                            bool required = false, bool repeatable = false)
        : OptionArgumentBase(name, optionsComment, repeatable, required, typeid(T), shortName,
                             valueName)
    {
    }

    [[nodiscard]] std::any parseToValue(const std::string &input) const override;

    OptionArgument<T> &withOptionsComment(std::string_view comment)
    {
        this->optionsComment = comment;
        return *this;
    }

    OptionArgument<T> &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    OptionArgument<T> &withShortName(std::string_view name)
    {
        this->shortName = name;
        return *this;
    }

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