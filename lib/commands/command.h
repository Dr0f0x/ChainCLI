#pragma once
#include <ostream>
#include <functional>
#include <memory>
#include <string_view>
#include <map>
#include "positional_argument.h"
#include "option_argument.h"
#include "flag_argument.h"
#include "cli_context.h"

namespace cli::commands
{
    class Command
    {
        friend std::ostream &operator<<(std::ostream &out, const Command &cmd);

    public:
        // Constructor initializes the identifier and description
        constexpr Command(std::string_view id, std::string_view short_desc, std::string_view long_desc, std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
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
        [[nodiscard]] const std::vector<std::unique_ptr<PositionalArgumentBase>> &getPositionalArguments() const noexcept { return positionalArguments; }
        [[nodiscard]] const std::vector<std::unique_ptr<OptionArgumentBase>> &getOptionArguments() const noexcept { return optionArguments; }
        [[nodiscard]] const std::vector<std::unique_ptr<FlagArgument>> &getFlagArguments() const noexcept { return flagArguments; }

        [[nodiscard]] std::string_view getDocStringShort() const;
        [[nodiscard]] std::string_view getDocStringLong() const;

        // Virtual destructor for base class
        virtual ~Command() = default;

        // try to run the passed callable
        void execute(const CliContext &context) const;

        // Generate documentation strings for the command and its arguments to avoid having to rebuilt them each time
        void buildDocStrings();

        Command &withShortDescription(std::string_view desc);
        Command &withLongDescription(std::string_view desc);

        template <typename T>
        Command &withPositionalArgument(std::unique_ptr<PositionalArgument<T>> arg)
        {
            positionalArguments.push_back(std::move(arg));
            return *this;
        }
        template <typename T>
        Command &withPositionalArgument(PositionalArgument<T> &&arg) { return withPositionalArgument(std::make_unique<PositionalArgument<T>>(std::move(arg))); }

        template <typename T>
        Command &withOptionArgument(std::unique_ptr<OptionArgument<T>> arg)
        {
            optionArguments.push_back(std::move(arg));
            return *this;
        }
        template <typename T>
        Command &withOptionArgument(OptionArgument<T> &&arg) { return withOptionArgument(std::make_unique<OptionArgument<T>>(std::move(arg))); }

        Command &withFlagArgument(std::unique_ptr<FlagArgument> arg)
        {
            flagArguments.push_back(std::move(arg));
            return *this;
        }
        Command &withFlagArgument(FlagArgument &&arg) { return withFlagArgument(std::make_unique<FlagArgument>(std::move(arg))); }

        Command &withExecutionFunc(std::unique_ptr<std::function<void(const CliContext &)>> actionPtr);
        Command &withExecutionFunc(std::function<void(const CliContext &)> &&action)
        {
            return withExecutionFunc(std::make_unique<std::function<void(const CliContext &)>>(std::move(action)));
        }

        Command &withSubCommand(std::unique_ptr<Command> subCommandPtr);
        Command &withSubCommand(Command &&subCommand) { return withSubCommand(std::make_unique<Command>(std::move(subCommand))); }

        Command *getSubCommand(std::string_view id);
        const Command *getSubCommand(std::string_view id) const;

        auto &getSubCommands() { return subCommands; }
        auto const &getSubCommands() const { return subCommands; }

    private:
        const std::string identifier;
        std::string shortDescription;
        std::string longDescription;
        std::unique_ptr<std::function<void(const CliContext &)>> executePtr;

        // arguments
        std::vector<std::unique_ptr<PositionalArgumentBase>> positionalArguments;
        std::vector<std::unique_ptr<OptionArgumentBase>> optionArguments;
        std::vector<std::unique_ptr<FlagArgument>> flagArguments;

        std::string docStringShort; // cached short doc string
        std::string docStringLong;  // cached long doc string

        std::map<std::string, std::unique_ptr<Command>, std::less<>> subCommands;
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