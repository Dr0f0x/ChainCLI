/*
 * Copyright 2025 Dominik Czekai
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include "argument_group.h"
#include "cli_context.h"
#include "flag_argument.h"
#include "option_argument.h"
#include "positional_argument.h"
#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <string_view>

namespace cli::commands
{

/// @brief Represents a command in the CLI application.
/// @details A command can have subcommands, arguments (positional, option, flag), and an execution function.
/// Commands can be nested to create a hierarchy of commands and subcommands.
class Command
{
    friend std::ostream &operator<<(std::ostream &out, const Command &cmd);

public:
    friend class cli::commands::docwriting::DocWriter;

    /// @brief Construct a new Command object.
    /// @param id The unique identifier for the command.
    /// @param short_desc A short description of the command.
    /// @param long_desc A long description of the command.
    /// @param actionPtr A pointer to the function to execute when the command is called.
    Command(std::string_view id, std::string_view short_desc, std::string_view long_desc,
            std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
        : identifier(id), shortDescription(short_desc), longDescription(long_desc),
          executePtr(std::move(actionPtr))
    {
    }

    /// @brief Construct a new Command object.
    /// @param id The unique identifier for the command.
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

    /// @brief Get the unique identifier for the command.
    /// @return The unique identifier for the command.
    [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept { return identifier; }

    /// @brief Get the short description of the command.
    /// @return The short description of the command.
    [[nodiscard]] constexpr std::string_view getShortDescription() const noexcept
    {
        return shortDescription;
    }

    /// @brief Get the long description of the command.
    /// @return The long description of the command.
    [[nodiscard]] constexpr std::string_view getLongDescription() const noexcept
    {
        return longDescription;
    }

    /// @brief Check if the command has an execution function.
    /// @return True if the command has an execution function, false otherwise.
    [[nodiscard]] constexpr bool hasExecutionFunction() const noexcept { return executePtr.get(); }

    /// @brief Get the positional arguments for the command.
    /// @return The positional arguments for the command.
    [[nodiscard]] const std::vector<std::shared_ptr<PositionalArgumentBase>> &
    getPositionalArguments() const noexcept
    {
        return positionalArguments;
    }

    /// @brief Get the option arguments for the command.
    /// @note Arguments appear on the command line help messages in the order they were added to the command.
    /// @return The option arguments for the command.
    [[nodiscard]] const std::vector<std::shared_ptr<OptionArgumentBase>> &getOptionArguments()
        const noexcept
    {
        return optionArguments;
    }

    /// @brief Get the flag arguments for the command.
    /// @note Arguments appear on the command line help messages in the order they were added to the command.
    /// @return The flag arguments for the command.
    [[nodiscard]] const std::vector<std::shared_ptr<FlagArgument>> &getFlagArguments()
        const noexcept
    {
        return flagArguments;
    }

    /// @brief Get the argument groups for the command.
    /// @note Argument groups appear on the command line help messages in the order they were added
    /// @return The argument groups for the command.
    [[nodiscard]] const std::vector<std::unique_ptr<ArgumentGroup>> &getArgumentGroups()
        const noexcept
    {
        return argumentGroups;
    }

    /// @brief Get the short documentation string for the command.
    /// @details This description only contains the textual representation of the command and its arguments as well as the short description.
    /// @note the doc strings are cached internally and need to be built before being accessed
    /// @return The short documentation string for the command.
    [[nodiscard]] std::string_view getDocStringShort() const;


    /// @brief Get the long documentation string for the command.
    /// @details This description contains the textual representation of the command and its arguments as well as the long description and the Options segment.
    /// @note the doc strings are cached internally and need to be built before being accessed
    /// @return The long documentation string for the command.
    [[nodiscard]] std::string_view getDocStringLong() const;

    /// @brief Get a sub-command by its identifier.
    /// @param id The identifier of the sub-command.
    /// @return A pointer to the sub-command if found, nullptr otherwise.
    [[nodiscard]] Command *getSubCommand(std::string_view id);

    /// @brief Get a sub-command by its identifier.
    /// @param id The identifier of the sub-command.
    /// @return A pointer to the sub-command if found, nullptr otherwise.
    [[nodiscard]] const Command *getSubCommand(std::string_view id) const;

    /// @brief Get all sub-commands of the command.
    /// @return A reference to the map of identifiers to their sub-commands.
    [[nodiscard]] auto &getSubCommands() { return subCommands; }

    /// @brief Get all sub-commands of the command.
    /// @return A reference to the map of identifiers to their sub-commands.
    [[nodiscard]] auto const &getSubCommands() const { return subCommands; }

#pragma endregion Accessor

    /// @brief Execute the command.
    /// @param context The CLI context to use for execution.
    void execute(const CliContext &context) const;

#pragma region ChainingMethods
    /// @brief Set the short description for the command.
    /// @param desc The short description to set.
    /// @return A reference to this command.
    Command &withShortDescription(std::string_view desc);


    /// @brief Set the long description for the command.
    /// @param desc The long description to set.
    /// @return A reference to this command.
    Command &withLongDescription(std::string_view desc);

    /// @brief Add a positional argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to the command.
    /// @tparam T The type of the positional argument.
    /// @param arg The positional argument to set.
    /// @return A reference to this command.
    template <typename T>
    Command &withPositionalArgument(std::shared_ptr<PositionalArgument<T>> arg)
    {
        safeAddToArgGroup(arg);
        positionalArguments.push_back(arg);
        return *this;
    }

    /// @brief Add a positional argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to the command.
    /// @tparam T The type of the positional argument.
    /// @param arg The positional argument to set.
    /// @return A reference to this command.
    template <typename T> Command &withPositionalArgument(PositionalArgument<T> &&arg)
    {
        return withPositionalArgument(std::make_shared<PositionalArgument<T>>(std::move(arg)));
    }

    /// @brief Add an option argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to
    /// @tparam T The type of the option argument.
    /// @param arg The option argument to set.
    /// @return A reference to this command.
    template <typename T> Command &withOptionArgument(std::shared_ptr<OptionArgument<T>> arg)
    {
        safeAddToArgGroup(arg);
        optionArguments.push_back(arg);
        return *this;
    }

    /// @brief Add an option argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to
    /// the command.
    /// @tparam T The type of the option argument.
    /// @param arg The option argument to set.
    /// @return A reference to this command.
    template <typename T> Command &withOptionArgument(OptionArgument<T> &&arg)
    {
        return withOptionArgument(std::make_shared<OptionArgument<T>>(std::move(arg)));
    }

    /// @brief Add a flag argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to
    /// the command.
    /// @param arg The flag argument to set.
    /// @return A reference to this command.
    Command &withFlagArgument(std::shared_ptr<FlagArgument> arg);

    /// @brief Add a flag argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to
    /// the command.
    /// @param arg The flag argument to set.
    /// @return A reference to this command.
    Command &withFlagArgument(FlagArgument &&arg);

    /// @brief Set the execution function for the command.
    /// @param action The function to execute when the command is called.
    /// @return A reference to this command.
    Command &withExecutionFunc(std::unique_ptr<std::function<void(const CliContext &)>> actionPtr);

    /// @brief Set the execution function for the command.
    /// @param action The function to execute when the command is called.
    /// @return A reference to this command.
    Command &withExecutionFunc(std::function<void(const CliContext &)> &&action);

    /// @brief Add a sub-command to this command.
    /// @param subCommandPtr The sub-command to add.
    /// @return A reference to this command.
    Command &withSubCommand(std::unique_ptr<Command> subCommandPtr);

    Command &withSubCommand(Command &&subCommand);

    /// @brief Add a sub-command to this command.
    /// @tparam ...Args The types of the sub-commands.
    /// @param ...args The sub-commands to add.
    /// @return A reference to this command.
    template <typename... Args>
        requires((sizeof...(Args) > 0) &&
                 (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
    Command &withExclusiveGroup(Args &&...args);

    /// @brief Add an inclusive argument group to this command.
    /// @tparam ...Args The types of the sub-commands.
    /// @param ...args The sub-commands to add.
    /// @return A reference to this command.
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

/// @brief Exception thrown when a command is malformed.
/// @details This can happen if the command is missing required arguments, has conflicting
/// arguments, or other issues that prevent it from being used correctly.
class MalformedCommandException : public std::runtime_error
{
public:
    explicit MalformedCommandException(const Command &cmd, const std::string &msg = "")
        : std::runtime_error(buildMessage(cmd, msg)), malformedCmd(&cmd)
    {
    }

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
    argumentGroups.emplace_back(std::make_unique<ExclusiveGroup>(std::forward<Args>(args)...));
    addArgGroup(*argumentGroups.back());
    indexForNewArgGroup++;
    return *this;
}

template <typename... Args>
    requires((sizeof...(Args) > 0) &&
             (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
inline Command &Command::withInclusiveGroup(Args &&...args)
{
    argumentGroups.emplace_back(std::make_unique<InclusiveGroup>(std::forward<Args>(args)...));
    addArgGroup(*argumentGroups.back());
    indexForNewArgGroup++;
    return *this;
}

} // namespace cli::commands