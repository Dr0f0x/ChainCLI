#pragma once
#include <memory>
#include <string_view>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include "command.h"

namespace cli::commands
{
    struct StringHash
    {
        using is_transparent = void;
        std::size_t operator()(std::string_view sv) const noexcept { return std::hash<std::string_view>{}(sv); }
        std::size_t operator()(const std::string &s) const noexcept { return std::hash<std::string_view>{}(s); }
        std::size_t operator()(const char *s) const noexcept { return std::hash<std::string_view>{}(s); }
    };

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
        struct Node
        {
            std::unique_ptr<Command> command;
            std::unordered_map<std::string, std::unique_ptr<Node>, StringHash, std::equal_to<>> children;

            explicit Node(std::unique_ptr<Command> cmd);

            void insertChild(std::unique_ptr<Command> cmd);
            Node *getChild(std::string_view id);
            auto &getChildren() { return children; }
            const Node *getChild(std::string_view id) const;
        };

        CommandTree();

        // Insert as child under a chain of parent ids
        template <typename... Ids>
        void insert(std::unique_ptr<Command> cmd, std::string_view parentId, Ids &&...rest)
        {
            Node *parent = findNode(parentId, std::forward<Ids>(rest)...);
            if (!parent)
            {
                std::vector<std::string> chain{std::string(parentId), std::string(rest)...};
                throw CommandNotFoundException(std::string(parentId), chain);
            }

            auto id = std::string(cmd->getIdentifier());
            parent->insertChild(std::move(cmd));
        }
        void insert(std::unique_ptr<Command> cmd) // insert at root
        {
            auto id = std::string(cmd->getIdentifier());
            root->insertChild(std::move(cmd));
        }

        // Lookup via identifier chain
        template <typename... Ids>
        Command *find(Ids &&...ids) const
        {
            return findRecursive(root.get(), std::forward<Ids>(ids)...);
        }

        // Recursive node lookup for insertion
        template <typename... Ids>
        Node *findNode(Ids &&...ids)
        {
            return findNodeRecursive(root.get(), std::forward<Ids>(ids)...);
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
        Node *getRoot() { return root.get(); }
        const Node *getRoot() const { return root.get(); }
        Command *getRootCommand() { return root->command.get(); }

    private:
        std::unique_ptr<Node> root;

        // Recursive finder
        template <typename... Ids>
        static Command *findRecursive(Node *node, std::string_view id, Ids &&...rest)
        {
            auto it = node->children.find(std::string(id));
            if (it == node->children.end())
                return nullptr;

            Node *child = it->second.get();
            Command *cmd = child->command.get();

            if constexpr (sizeof...(rest) == 0)
            {
                // Base case: no more ids, return the child
                return cmd;
            }
            // Recursive case: continue with remaining ids
            return findRecursive(child, std::forward<Ids>(rest)...);
        }

        static Command *findRecursive(const Node *node);

        template <typename... Ids>
        static Node *findNodeRecursive(Node *node, std::string_view id, Ids &&...rest)
        {
            auto it = node->children.find(std::string(id));
            if (it == node->children.end())
                return nullptr;

            Node *child = it->second.get();

            if constexpr (sizeof...(rest) == 0)
            {
                // Base case: no more ids, return the child
                return child;
            }
            // Recursive case: continue with remaining ids
            return findNodeRecursive(child, std::forward<Ids>(rest)...);
        }
        static Node *findNodeRecursive(Node *node);

        // Recursive DFS helper
        static void forEachCommandRecursive(const Node *node, const std::function<void(Command &)> &func)
        {
            if (node->command)
            {
                func(*node->command); // call user-provided function
            }
            for (const auto &[key, child] : node->children)
            {
                forEachCommandRecursive(child.get(), func);
            }
        }

        // Pretty-print helper
        static void printRecursive(std::ostream &os, const Node *node, int indent);
    };
} // namespace cli::commands
