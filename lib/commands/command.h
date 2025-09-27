#pragma once
#include <ostream>
#include <functional>
#include <memory>
#include <string_view>

namespace cli::commands
{

    class Command
    {
        friend std::ostream &operator<<(std::ostream &out, const Command &cmd);

    public:
        // Constructor initializes the identifier and description
        constexpr Command(std::string_view id, std::string_view short_desc, std::string_view long_desc, std::unique_ptr<std::function<void()>> actionPtr)
            : identifier(id), shortDescription(short_desc), longDescription(long_desc), executePtr(std::move(actionPtr)) {}

        // Accessors
        [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept { return identifier; }
        [[nodiscard]] constexpr std::string_view getShortDescription() const noexcept { return shortDescription; }
        [[nodiscard]] constexpr std::string_view getLongDescription() const noexcept { return longDescription; }

        // Virtual destructor for base class
        virtual ~Command() = default;

        // try to run the passed callable
        void execute() const;

    private:
        const std::string_view identifier;
        const std::string_view shortDescription;
        const std::string_view longDescription;
        std::unique_ptr<std::function<void()>> executePtr;
    };
} // namespace cli::commands

template <>
struct std::formatter<cli::commands::Command> : std::formatter<std::string>
{
    auto format(const cli::commands::Command& cmd, format_context &ctx) const
    {
        return formatter<string>::format(
            std::format("Command - {} ( short Description: {} ; long Description: {} )",
                        cmd.getIdentifier(),
                        cmd.getLongDescription(),
                        cmd.getShortDescription()),
            ctx);
    }
};