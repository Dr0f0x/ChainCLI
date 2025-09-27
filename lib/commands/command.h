#pragma once
#include <ostream>
#include <functional>
#include <string_view>

namespace cli::commands
{

    class Command
    {
        friend std::ostream &operator<<(std::ostream &out, const Command &cmd);

    public:
        // Constructor initializes the identifier and description
        constexpr Command(std::string_view id, std::string_view desc, std::function<void()> const& action)
            : identifier(id), description(desc), executeFunc(action) {}

        // Accessors
        [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept { return identifier; }
        [[nodiscard]] constexpr std::string_view getDescription() const noexcept { return description; }

        // Virtual destructor for base class
        virtual ~Command() = default;

        // try to run the passed callable
        void execute() const;

    private:
        const std::string_view identifier;
        const std::string_view description;
        std::function<void()> const& executeFunc;
    };
} // namespace cli::commands

template <>
struct std::formatter<cli::commands::Command> : std::formatter<std::string>
{
    auto format(const cli::commands::Command& cmd, format_context &ctx) const
    {
        return formatter<string>::format(
            std::format("Command - {} ( Description: {} )",
                        cmd.getIdentifier(),
                        cmd.getDescription()),
            ctx);
    }
};