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
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

#include "command.h"

namespace cli::commands
{

/// @brief Exception thrown when a command is not found in the command tree.
class CommandNotFoundException : public std::runtime_error
{
public:
    /// @brief Construct a new CommandNotFoundException.
    /// @param missingId The ID of the missing command.
    /// @param chain The chain of command IDs leading to the missing command.
    CommandNotFoundException(const std::string &missingId, const std::vector<std::string> &chain)
        : std::runtime_error(buildMessage(missingId, chain)), missing(missingId), path(chain)
    {
    }

    const std::string &missingId() const noexcept { return missing; }

    const std::vector<std::string> &insertChain() const noexcept { return path; }

private:
    std::string missing;
    std::vector<std::string> path;

    static std::string buildMessage(const std::string &id, const std::vector<std::string> &chain);
};

/// @brief Tree structure to manage commands and their subcommands.
/// @details Each node in the tree is a Command, and commands can have multiple subcommands.
/// The tree allows for insertion, lookup, and traversal of commands.
class CommandTree
{
public:
    /// @brief Construct a new CommandTree.
    /// @param rootName The name of the root command.
    explicit CommandTree(std::string_view rootName);

    /// @brief Insert a command into the tree.
    /// @tparam ...Ids The types of the parent command IDs.
    /// @param cmd The command to insert.
    /// @param parentId The ID of the direct parent command (used to split first value of ...rest).
    /// @param ...rest The IDs of any additional parent commands.
    template <typename... Ids>
    void insert(std::unique_ptr<Command> cmd, std::string_view parentId, Ids &&...rest)
    {
        Command *parentCommandPtr = find(parentId, std::forward<Ids>(rest)...);
        if (!parentCommandPtr)
        {
            std::vector<std::string> chain{std::string(parentId), std::string(rest)...};
            throw CommandNotFoundException(std::string(parentId), chain);
        }

        parentCommandPtr->withSubCommand(std::move(cmd));
    }

    /// @brief Insert a command into the tree.
    /// @param cmd The command to insert.
    void insert(std::unique_ptr<Command> cmd) // insert at root
    {
        root->withSubCommand(std::move(cmd));
    }

    /// @brief Find a command in the tree by a path of identifiers leading to it.
    /// @tparam ...Ids The types of the command IDs.
    /// @param ...ids The command IDs.
    /// @return A pointer to the found command, or nullptr if not found.
    template <typename... Ids> Command *find(Ids &&...ids) const
    {
        return findRecursive(root.get(), std::forward<Ids>(ids)...);
    }

    /// @brief Apply a function to each command in the tree.
    /// @details The function is applied in a depth-first search (DFS) manner.
    /// @param func The function to apply.
    void forEachCommand(const std::function<void(Command *)> &func) const
    {
        if (root)
        {
            forEachCommandRecursive(root.get(), func);
        }
    }

    /// @brief Apply a function to each command in the tree.
    /// @details The function is applied in a depth-first search (DFS) manner.
    /// @param func The function to apply.
    void forEachCommand(const std::function<void(Command &)> &func) const
    {
        if (root)
        {
            forEachCommandRecursive(root.get(), func);
        }
    }

    /// @brief Get the root command of the tree.
    /// @return A pointer to the root command.
    Command *getRootCommand() { return root.get(); }

    /// @brief Get the root command of the tree.
    /// @return A pointer to the root command.
    const Command *getRootCommand() const { return root.get(); }

    /// @brief Get the path for a command in the tree.
    /// @note Uses a pre-built map for O(1) lookup internally that maps needs to be constructed first using the buildCommandPathMap function.
    /// @param cmd The command to find the path for.
    /// @return The path to the command, or an empty string if not found.
    std::string_view getPathForCommand(Command *cmd) const;

    /// @brief Build a map of command paths for quick lookup.
    /// @param separator The separator to use between command names in the path (default is a space).
    void buildCommandPathMap(const std::string &separator = " ");

private:
    std::unique_ptr<Command> root;
    std::unordered_map<Command *, std::string> commandPathMap;

    void buildCommandPathMapRecursive(Command *cmd, std::vector<std::string> &path,
                                      const std::string &separator);

    // Recursive finder
    template <typename... Ids>
    static Command *findRecursive(Command *cmdPtr, std::string_view id, Ids &&...rest)
    {
        auto it = cmdPtr->getSubCommands().find(std::string(id));
        if (it == cmdPtr->getSubCommands().end())
            return nullptr;

        Command *subCommandPtr = it->second.get();

        if constexpr (sizeof...(rest) == 0)
        {
            // Base case: no more ids, return the child
            return subCommandPtr;
        }
        // Recursive case: continue with remaining ids
        return findRecursive(subCommandPtr, std::forward<Ids>(rest)...);
    }

    static Command *findRecursive(Command *cmdPtr) { return cmdPtr; }

    // Recursive DFS helper
    static void forEachCommandRecursive(Command *cmdPtr, const std::function<void(Command *)> &func)
    {
        if (cmdPtr)
        {
            func(cmdPtr); // call user-provided function
            for (const auto &[key, subCommandPtr] : cmdPtr->getSubCommands())
            {
                forEachCommandRecursive(subCommandPtr.get(), func);
            }
        }
    }

    static void forEachCommandRecursive(Command *cmdPtr, const std::function<void(Command &)> &func)
    {
        if (cmdPtr)
        {
            func(*cmdPtr); // call user-provided function
            for (const auto &[key, subCommandPtr] : cmdPtr->getSubCommands())
            {
                forEachCommandRecursive(subCommandPtr.get(), func);
            }
        }
    }
};
} // namespace cli::commands
