#pragma once
#include <ostream>
#include <functional>
#include <memory>
#include <string_view>
#include <map>
#include "positional_argument.h"
#include "option_argument.h"
#include "flag_argument.h"
#include "argument_group.h"
#include "cli_context.h"

namespace cli::commands
{
    class Command
    {
        friend std::ostream &operator<<(std::ostream &out, const Command &cmd);

    public:
        // Constructor initializes the identifier and description
        Command(std::string_view id, std::string_view short_desc, std::string_view long_desc, std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
            : identifier(id), shortDescription(short_desc), longDescription(long_desc), executePtr(std::move(actionPtr))
        {
        }
        explicit Command(std::string_view id)
            : identifier(id), shortDescription(""), longDescription(""), executePtr(nullptr)
        {
        }

        // Movable
        Command(Command &&) noexcept = default;
        Command &operator=(Command &&) noexcept = default;

        // Non-copyable
        Command(const Command &) = delete;
        Command &operator=(const Command &) = delete;

        virtual ~Command() = default;

#pragma region Accessors
        [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept { return identifier; }
        [[nodiscard]] constexpr std::string_view getShortDescription() const noexcept { return shortDescription; }
        [[nodiscard]] constexpr std::string_view getLongDescription() const noexcept { return longDescription; }
        [[nodiscard]] constexpr bool hasExecutionFunction() const noexcept { return executePtr.get(); }

        [[nodiscard]] const std::vector<std::shared_ptr<PositionalArgumentBase>> &getPositionalArguments() const noexcept { return positionalArguments; }
        [[nodiscard]] const std::vector<std::shared_ptr<OptionArgumentBase>> &getOptionArguments() const noexcept { return optionArguments; }
        [[nodiscard]] const std::vector<std::shared_ptr<FlagArgument>> &getFlagArguments() const noexcept { return flagArguments; }
        [[nodiscard]] const std::vector<std::unique_ptr<ArgumentGroup>> &getArgumentGroups() const noexcept { return argumentGroups; }

        [[nodiscard]] std::string_view getDocStringShort() const;
        [[nodiscard]] std::string_view getDocStringLong() const;

        [[nodiscard]] Command *getSubCommand(std::string_view id);
        [[nodiscard]] const Command *getSubCommand(std::string_view id) const;

        [[nodiscard]] auto &getSubCommands() { return subCommands; }
        [[nodiscard]] auto const &getSubCommands() const { return subCommands; }
#pragma endregion Accessor

        // try to run the passed callable
        void execute(const CliContext &context) const;

        // Generate documentation strings for the command and its arguments to avoid having to rebuilt them each time
        void buildDocStrings(std::string_view fullCommandPath);

#pragma region ChainingMethods
        Command &withShortDescription(std::string_view desc);
        Command &withLongDescription(std::string_view desc);

        template <typename T>
        Command &withPositionalArgument(std::shared_ptr<PositionalArgument<T>> arg)
        {
            safeAddToArgGroup(arg);
            positionalArguments.push_back(arg);
            return *this;
        }
        template <typename T>
        Command &withPositionalArgument(PositionalArgument<T> &&arg) { return withPositionalArgument(std::make_shared<PositionalArgument<T>>(std::move(arg))); }

        template <typename T>
        Command &withOptionArgument(std::shared_ptr<OptionArgument<T>> arg)
        {
            safeAddToArgGroup(arg);
            optionArguments.push_back(arg);
            return *this;
        }
        template <typename T>
        Command &withOptionArgument(OptionArgument<T> &&arg) { return withOptionArgument(std::make_shared<OptionArgument<T>>(std::move(arg))); }

        Command &withFlagArgument(std::shared_ptr<FlagArgument> arg)
        {
            safeAddToArgGroup(arg);
            flagArguments.push_back(arg);
            return *this;
        }
        Command &withFlagArgument(FlagArgument &&arg) { return withFlagArgument(std::make_shared<FlagArgument>(std::move(arg))); }

        Command &withExecutionFunc(std::unique_ptr<std::function<void(const CliContext &)>> actionPtr);
        Command &withExecutionFunc(std::function<void(const CliContext &)> &&action)
        {
            return withExecutionFunc(std::make_unique<std::function<void(const CliContext &)>>(std::move(action)));
        }

        Command &withSubCommand(std::unique_ptr<Command> subCommandPtr);
        Command &withSubCommand(Command &&subCommand) { return withSubCommand(std::make_unique<Command>(std::move(subCommand))); }

        template <typename... Args>
            requires((sizeof...(Args) > 0) &&
                     (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
        Command &withExclusiveGroup(Args &&...args);

        template <typename... Args>
            requires((sizeof...(Args) > 0) &&
                     (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
        Command &withInclusiveGroup(Args &&...args);
#pragma endregion ChainingMethods

    private:
        size_t indexForNewArgGroup{0};
        void safeAddToArgGroup(const std::shared_ptr<ArgumentBase> &arg);
        void addArgGroup(const ArgumentGroup &argGroup);

        std::string identifier;
        std::string shortDescription;
        std::string longDescription;
        std::unique_ptr<std::function<void(const CliContext &)>> executePtr;

        // arguments
        std::vector<std::shared_ptr<PositionalArgumentBase>> positionalArguments;
        std::vector<std::shared_ptr<OptionArgumentBase>> optionArguments;
        std::vector<std::shared_ptr<FlagArgument>> flagArguments;
        std::vector<std::unique_ptr<ArgumentGroup>> argumentGroups;

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

    template <typename... Args>
        requires((sizeof...(Args) > 0) &&
                 (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
    inline Command &Command::withExclusiveGroup(Args &&...args)
    {
        argumentGroups.emplace_back(
            std::make_unique<ExclusiveGroup>(std::forward<Args>(args)...));
        addArgGroup(*argumentGroups.back());
        indexForNewArgGroup++;
        return *this;
    }

    template <typename... Args>
        requires((sizeof...(Args) > 0) &&
                 (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
    inline Command &Command::withInclusiveGroup(Args &&...args)
    {
        argumentGroups.emplace_back(
            std::make_unique<InclusiveGroup>(std::forward<Args>(args)...));
        addArgGroup(*argumentGroups.back());
        indexForNewArgGroup++;
        return *this;
    }
} // namespace cli::commands