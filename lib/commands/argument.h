#pragma once
#include <ostream>
#include <string>

namespace cli::commands
{
    class Argument
    {
        friend std::ostream &operator<<(std::ostream &out, const Argument &arg);

    public:
        constexpr Argument(const std::string &name, const std::string &usage_comment)
            : name(name), usageComment(usage_comment) {}

        [[nodiscard]] constexpr std::string_view getName() const noexcept { return name; }
        [[nodiscard]] constexpr std::string_view getUsageComment() const noexcept { return usageComment; }

        virtual ~Argument() = default;

    private:
        const std::string name;
        const std::string usageComment;
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