#pragma once
#include <ostream>
#include <functional>
#include <memory>
#include <string_view>
#include "argument.h"

namespace cli::commands
{

    class Command
    {
        friend std::ostream &operator<<(std::ostream &out, const Command &cmd);

    public:
        // Constructor initializes the identifier and description
        constexpr Command(std::string const& id, std::string const& short_desc, std::string const& long_desc, std::unique_ptr<std::function<void()>> actionPtr)
            : identifier(id), shortDescription(short_desc), longDescription(long_desc), executePtr(std::move(actionPtr)) {}

        // Accessors
        [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept { return identifier; }
        [[nodiscard]] constexpr std::string_view getShortDescription() const noexcept { return shortDescription; }
        [[nodiscard]] constexpr std::string_view getLongDescription() const noexcept { return longDescription; }
        [[nodiscard]] const std::vector<std::unique_ptr<Argument>>& getArguments() const noexcept { return arguments; }

        // Virtual destructor for base class
        virtual ~Command() = default;

        // try to run the passed callable
        void execute() const;
        Command& withArgument(std::unique_ptr<Argument>& arg);
    private:
        const std::string identifier;
        const std::string shortDescription;
        const std::string longDescription;
        std::unique_ptr<std::function<void()>> executePtr;
        std::vector<std::unique_ptr<Argument>> arguments;
    };
} // namespace cli::commands

template <>
struct std::formatter<cli::commands::Command> : std::formatter<std::string>
{
    auto format(const cli::commands::Command& cmd, format_context &ctx) const
    {
        std::string argsStr;
        auto& args = cmd.getArguments();
        for (size_t i = 0; i < args.size(); ++i) {
            if (args[i])
                argsStr += std::format("{}", *(args[i])); // requires operator<< or std::formatter for Argument
            if (i + 1 < args.size())
                argsStr += ", ";
        }

        return formatter<string>::format(
            std::format("Command - {} ( short Description: {} ; long Description: {} ; arguments: [{}] )",
                        cmd.getIdentifier(),
                        cmd.getShortDescription(),
                        cmd.getLongDescription(),
                        argsStr),
            ctx);
    }
};