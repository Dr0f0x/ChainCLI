#pragma once
#include <ostream>
#include <string>
#include <any>
#include <typeindex>
#include <string_view>
#include "argument.h"

namespace cli::commands
{

    template <typename T>
    class OptionArgument : public ArgumentBase
    {
        friend std::ostream &operator<<(std::ostream &out, const OptionArgument<T> &arg)
        {
            out << arg.name << " (" << arg.usageComment << ")";
            return out;
        }

    public:
        explicit OptionArgument(std::string_view name,
                                    std::string_view short_name = "",
                                    std::string_view usage_comment = "",
                                    bool required = true)
            : ArgumentBase(name, usage_comment, required, typeid(T)),
              shortName(short_name) {}

        // Movable
        OptionArgument(OptionArgument &&) noexcept = default;
        OptionArgument &operator=(OptionArgument &&) noexcept = default;

        [[nodiscard]] constexpr std::string_view getShortName() const noexcept { return shortName; }

        [[nodiscard]] std::any parseToValue(const std::string &input) const override;

        std::string getOptionsDocString() const override;
        std::string getArgDocString() const override;

        PositionalArgument<T> &withShortName(std::string_view short_name)
        {
            shortName = short_name;
            return *this;
        }

        OptionArgument<T> &withUsageComment(std::string_view usage_comment)
        {
            usageComment = usage_comment;
            return *this;
        }

        OptionArgument<T> &withRequired(bool req)
        {
            required = req;
            return *this;
        }

    private:
        std::string shortName;
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