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
class PositionalArgumentBase : public TypedArgumentBase, public ArgumentBase
{
public:
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

template <typename T> class PositionalArgument : public PositionalArgumentBase
{
    friend std::ostream &operator<<(std::ostream &out, const PositionalArgument<T> &arg)
    {
        out << arg.name << " (" << arg.optionsComment << ")";
        return out;
    }

public:
    explicit PositionalArgument(std::string_view name, std::string_view optionsComment = "",
                                bool required = true, bool repeatable = false)
        : PositionalArgumentBase(name, optionsComment, repeatable, required, typeid(T))
    {
    }

    [[nodiscard]] std::any parseToValue(const std::string &input) const override;

    PositionalArgument<T> &withOptionsComment(std::string_view comment)
    {
        optionsComment = comment;
        return *this;
    }

    PositionalArgument<T> &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    PositionalArgument<T> &withShortName(std::string_view name)
    {
        this->shortName = name;
        return *this;
    }

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