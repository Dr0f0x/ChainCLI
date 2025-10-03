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
                           std::string_view optionsComment,
                           bool required,
                           std::type_index t,
                           std::string_view shortName,
                           std::string_view valueName)
            : TypedArgumentBase(name, optionsComment, ArgumentKind::Option, required, t),
              FlaggedArgumentBase(shortName),
              valueName(valueName) {}

        [[nodiscard]] std::string getOptionsDocString() const override;
        [[nodiscard]] std::string getArgDocString() const override;

        [[nodiscard]] constexpr std::string_view getValueName() const noexcept { return valueName; }

    protected:
        std::string valueName;
    };

    template <typename T>
    class OptionArgument : public OptionArgumentBase
    {
        friend std::ostream &operator<<(std::ostream &out, const OptionArgument<T> &arg)
        {
            out << arg.name << " (" << arg.optionsComment << ")";
            return out;
        }

    public:
        explicit OptionArgument(std::string_view name,
                                std::string_view valueName,
                                std::string_view shortName = "",
                                std::string_view optionsComment = "",
                                bool required = false)
            : OptionArgumentBase(name, optionsComment, required, typeid(T), shortName, valueName) {}

        // Movable
        OptionArgument(OptionArgument &&) noexcept = default;
        OptionArgument &operator=(OptionArgument &&) noexcept = default;

        [[nodiscard]] std::any parseToValue(const std::string &input) const override;

        OptionArgument<T> &withOptionsComment(std::string_view usage_comment)
        {
            optionsComment = usage_comment;
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
}