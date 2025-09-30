#pragma once
#include <ostream>
#include <functional>
#include <memory>
#include <string_view>
#include "argument.h"
#include "cli_context.h"

namespace cli::commands
{
    class Command
    {
        friend std::ostream &operator<<(std::ostream &out, const Command &cmd);

    public:
        // Constructor initializes the identifier and description
        constexpr Command(std::string_view id, std::string_view short_desc, std::string_view long_desc, std::unique_ptr<std::function<void(const CliContext&)>> actionPtr)
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
        [[nodiscard]] const std::vector<std::unique_ptr<ArgumentBase>> &getArguments() const noexcept { return arguments; }

        [[nodiscard]] std::string_view getDocStringShort() const;
        [[nodiscard]] std::string_view getDocStringLong() const;

        void setSubCommandCallback(std::unique_ptr<std::function<void(std::unique_ptr<Command>)>> cb) { subCommandCallBack = std::move(cb); }

        // Virtual destructor for base class
        virtual ~Command() = default;

        // try to run the passed callable
        void execute(const CliContext &context) const;

        // Generate documentation strings for the command and its arguments to avoid having to rebuilt them each time
        void buildDocStrings();

        Command &withShortDescription(std::string_view desc);
        Command &withLongDescription(std::string_view desc);

        template <typename T>
        Command &withArgument(std::unique_ptr<PositionalArgument<T>> arg)
        {
            arguments.push_back(std::move(arg));
            return *this;
        }
        template <typename T>
        Command &withArgument(PositionalArgument<T> &&arg) { return withArgument(std::make_unique<PositionalArgument<T>>(std::move(arg))); }
        template <typename T>
        Command &withArgument(PositionalArgument<T> &arg) { return withArgument(std::make_unique<PositionalArgument<T>>(std::move(arg))); }

        Command &withExecutionFunc(std::unique_ptr<std::function<void(const CliContext &)>> actionPtr);
        Command &withExecutionFunc(std::function<void(const CliContext &)> &&actionPtr)
        {
            return withExecutionFunc(std::make_unique<std::function<void(const CliContext &)>>(std::move(actionPtr)));
        }
        Command &withExecutionFunc(std::function<void(const CliContext &)> &actionPtr)
        {
            return withExecutionFunc(std::make_unique<std::function<void(const CliContext &)>>(std::move(actionPtr)));
        }

        Command &withSubCommand(std::unique_ptr<Command> subCommandPtr);
        Command &withSubCommand(Command &&subCommand) { return withSubCommand(std::make_unique<Command>(std::move(subCommand))); }
        Command &withSubCommand(Command &subCommand) { return withSubCommand(std::make_unique<Command>(std::move(subCommand))); }

    private:
        const std::string identifier;
        std::string shortDescription;
        std::string longDescription;
        std::unique_ptr<std::function<void(const CliContext &)>> executePtr;
        std::vector<std::unique_ptr<ArgumentBase>> arguments;

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