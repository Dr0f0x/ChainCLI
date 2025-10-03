#pragma once
#include <ostream>
#include <string>
#include <any>
#include <typeindex>
#include <string_view>
#include "argument.h"
#include "parsing/parser_utils.h"

namespace cli::commands
{
    class OptionArgumentBase : public TypedArgumentBase, public FlaggedArgumentBase
    {
    public:
        OptionArgumentBase(std::string_view name,
                           std::string_view usage_comment,
                           bool required,
                           std::type_index t,
                           std::string_view shortName,
                           std::string_view valueName)
            : TypedArgumentBase(name, usage_comment, ArgumentKind::Option, required, t),
              FlaggedArgumentBase(shortName),
              valueName(valueName) {}

    protected:
        std::string valueName;
    };

    template <typename T>
    class OptionArgument : public OptionArgumentBase
    {
        friend std::ostream &operator<<(std::ostream &out, const OptionArgument<T> &arg)
        {
            out << arg.name << " (" << arg.usageComment << ")";
            return out;
        }

    public:
        explicit OptionArgument(std::string_view name,
                                std::string_view valueName,
                                std::string_view shortName = "",
                                std::string_view usage_comment = "",
                                bool required = false)
            : OptionArgumentBase(name, usage_comment, required, typeid(T), shortName, valueName) {}

        // Movable
        OptionArgument(OptionArgument &&) noexcept = default;
        OptionArgument &operator=(OptionArgument &&) noexcept = default;

        [[nodiscard]] std::any parseToValue(const std::string &input) const override;

        std::string getOptionsDocString() const override;
        std::string getArgDocString() const override;

        OptionArgument<T> &withOptionsComment(std::string_view usage_comment)
        {
            usageComment = usage_comment;
            return *this;
        }

        OptionArgument<T> &withRequired(bool req)
        {
            required = req;
            return *this;
        }
    };

    template <typename T>
    inline std::any OptionArgument<T>::parseToValue(const std::string &input) const
    {
        return cli::parsing::ParseHelper::parse<T>(input);
    }

    template <typename T>
    inline std::string OptionArgument<T>::getOptionsDocString() const
    {
        return "Options doc string for " + std::string(name);
    }

    template <typename T>
    inline std::string OptionArgument<T>::getArgDocString() const
    {
        return "Argument doc string for " + std::string(name);
    }
}