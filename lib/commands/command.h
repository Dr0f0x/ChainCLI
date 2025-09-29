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
        constexpr Command(std::string_view id, std::string_view short_desc, std::string_view long_desc, std::unique_ptr<std::function<void()>> actionPtr)
            : identifier(id), shortDescription(short_desc), longDescription(long_desc), executePtr(std::move(actionPtr)) {}
        explicit constexpr Command(std::string_view id)
            : identifier(id), shortDescription(""), longDescription(""), executePtr(nullptr) {}

        // Movable
        Command(Command &&) noexcept = default;
        Command &operator=(Command &&) noexcept = default;

        // Non-copyable
        Command(const Command &) = delete;
        Command &operator=(const Command &) = delete;

        // Accessors
        [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept { return identifier; }
        [[nodiscard]] constexpr std::string_view getShortDescription() const noexcept { return shortDescription; }
        [[nodiscard]] constexpr std::string_view getLongDescription() const noexcept { return longDescription; }
        [[nodiscard]] const std::vector<std::unique_ptr<Argument>> &getArguments() const noexcept { return arguments; }

        [[nodiscard]] std::string_view getDocStringShort() const;
        [[nodiscard]] std::string_view getDocStringLong() const;

        void setSubCommandCallback(std::unique_ptr<std::function<void(std::unique_ptr<Command>)>> cb) { subCommandCallBack = std::move(cb); }

        // Virtual destructor for base class
        virtual ~Command() = default;

        // try to run the passed callable
        void execute() const;

        // Generate documentation strings for the command and its arguments to avoid having to rebuilt them each time
        void buildDocStrings();

        Command &withShortDescription(std::string_view desc);
        Command &withLongDescription(std::string_view desc);

        Command &withArgument(std::unique_ptr<Argument> arg);
        Command &withArgument(Argument &&arg) { return withArgument(std::make_unique<Argument>(std::move(arg))); }
        Command &withArgument(Argument &arg) { return withArgument(std::make_unique<Argument>(std::move(arg))); }

        Command &withExecutionFunc(std::unique_ptr<std::function<void()>> actionPtr);
        Command &withExecutionFunc(std::function<void()> &&actionPtr) { return withExecutionFunc(std::make_unique<std::function<void()>>(std::move(actionPtr))); }
        Command &withExecutionFunc(std::function<void()> &actionPtr) { return withExecutionFunc(std::make_unique<std::function<void()>>(std::move(actionPtr))); }

        Command &withSubCommand(std::unique_ptr<Command> subCommandPtr);
        Command &withSubCommand(Command &&subCommand) { return withSubCommand(std::make_unique<Command>(std::move(subCommand))); }
        Command &withSubCommand(Command &subCommand) { return withSubCommand(std::make_unique<Command>(std::move(subCommand))); }

    private:
        const std::string identifier;
        std::string shortDescription;
        std::string longDescription;
        std::unique_ptr<std::function<void()>> executePtr;
        std::vector<std::unique_ptr<Argument>> arguments;

        std::string docStringShort; // cached short doc string
        std::string docStringLong;  // cached long doc string

        std::unique_ptr<std::function<void(std::unique_ptr<Command>)>> subCommandCallBack; // used to insert subcommands into the same tree
    };

    class MalformedCommandException : public std::runtime_error
    {
    public:
        explicit MalformedCommandException(const Command &cmd, const std::string &msg = "")
            : std::runtime_error(buildMessage(cmd, msg)), malformedCmd(&cmd) {}

        const Command &command() const noexcept { return *malformedCmd; }

    private:
        const Command *malformedCmd;

        static std::string buildMessage(const Command &cmd, const std::string &msg);
    };
} // namespace cli::commands

template <>
struct std::formatter<cli::commands::Command> : std::formatter<std::string>
{
    auto format(const cli::commands::Command &cmd, format_context &ctx) const
    {
        std::string argsStr;
        auto &args = cmd.getArguments();
        for (size_t i = 0; i < args.size(); ++i)
        {
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