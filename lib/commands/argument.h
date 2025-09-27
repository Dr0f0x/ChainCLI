#pragma once
#include <ostream>
#include <string>

namespace cli::commands
{
    class Argument
    {
        friend std::ostream &operator<<(std::ostream &out, const Argument &arg);

    public:
        constexpr Argument(std::string_view name, std::string_view short_name, const std::string_view &usage_comment, bool required)
            : name(name), shortName(short_name), usageComment(usage_comment), required(required) {}
        explicit constexpr Argument(std::string_view name)
            : name(name), shortName(""), usageComment("") {}

        [[nodiscard]] constexpr std::string_view getName() const noexcept { return name; }
        [[nodiscard]] constexpr std::string_view getUsageComment() const noexcept { return usageComment; }

        virtual ~Argument() = default;

        bool hasShortName() const { return !shortName.empty(); }

        //documentation to be used under Options for command
        [[nodiscard]] std::string_view getOptionsDocString() const;

        //documentation to be used in the command display (example command display: run <--help> [args] etc)
        [[nodiscard]] std::string_view getArgDocString() const;
    private:
        std::string name;
        std::string shortName;
        std::string usageComment;
        bool required{true};
    };
} // namespace cli::commands

template <>
struct std::formatter<cli::commands::Argument> : std::formatter<std::string>
{
    auto format(const cli::commands::Argument &arg, std::format_context &ctx) const
    {
        return formatter<std::string>::format(
            std::format("{} ({})", arg.getName(), arg.getUsageComment()),
            ctx);
    }
};