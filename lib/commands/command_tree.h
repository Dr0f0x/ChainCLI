#pragma once
#include <memory>
#include <string_view>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include "command.h"

namespace cli::commands
{
    class CommandNotFoundException : public std::runtime_error
    {
    public:
        CommandNotFoundException(const std::string &missingId, const std::vector<std::string> &chain)
            : std::runtime_error(buildMessage(missingId, chain)), missing(missingId), path(chain) {}

        const std::string &missingId() const noexcept { return missing; }
        const std::vector<std::string> &insertChain() const noexcept { return path; }

    private:
        std::string missing;
        std::vector<std::string> path;

        static std::string buildMessage(const std::string &id, const std::vector<std::string> &chain);
    };

    class CommandTree
    {
    public:
        explicit CommandTree(std::string_view rootName);

        // Insert as child under a chain of parent ids
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
        void insert(std::unique_ptr<Command> cmd) // insert at root
        {
            root->withSubCommand(std::move(cmd));
        }

        // Lookup via identifier chain
        template <typename... Ids>
        Command *find(Ids &&...ids) const
        {
            return findRecursive(root.get(), std::forward<Ids>(ids)...);
        }

        // Iterate over all commands in the tree (DFS)
        void forEachCommand(const std::function<void(Command &)> &func) const
        {
            if (root)
            {
                forEachCommandRecursive(root.get(), func);
            }
        }

        // Print tree
        void print(std::ostream &os, int indent = 0) const;
        Command *getRootCommand() { return root.get(); }
        const Command *getRootCommand() const { return root.get(); }

    private:
        std::unique_ptr<Command> root;

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

        // Pretty-print helper
        static void printRecursive(std::ostream &os, const Command *cmdPtr, bool last, int indent);
    };
} // namespace cli::commands
