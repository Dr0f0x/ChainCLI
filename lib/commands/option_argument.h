#pragma once
#include <ostream>
#include <string>
#include <any>
#include <typeindex>
#include <string_view>
#include "argument.h"

namespace cli::commands
{
    class OptionArgument : public ArgumentBase
    {
        friend std::ostream &operator<<(std::ostream &out, const OptionArgument &arg)
        {
            out << arg.name << " (" << arg.usageComment << ")";
            return out;
        }

    public:
        explicit OptionArgument(std::string_view name,
                                std::string_view short_name = "",
                                std::string_view usage_comment = "",
                                bool required = false)
            : ArgumentBase(name, usage_comment, required),
              shortName(short_name) {}

        // Movable
        OptionArgument(OptionArgument &&) noexcept = default;
        OptionArgument &operator=(OptionArgument &&) noexcept = default;

        [[nodiscard]] constexpr std::string_view getShortName() const noexcept { return shortName; }

        std::string getOptionsDocString() const override;
        std::string getArgDocString() const override;

        OptionArgument &withShortName(std::string_view short_name)
        {
            shortName = short_name;
            return *this;
        }

        OptionArgument &withUsageComment(std::string_view usage_comment)
        {
            usageComment = usage_comment;
            return *this;
        }

        OptionArgument &withRequired(bool req)
        {
            required = req;
            return *this;
        }

    private:
        std::string shortName;
    };
}