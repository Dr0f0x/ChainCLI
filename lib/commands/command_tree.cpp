#include "command_tree.h"
#include <sstream>

namespace cli::commands
{
    CommandTree::Node::Node(std::unique_ptr<Command> cmd)
        : command(std::move(cmd)) {}

    void CommandTree::Node::insertChild(std::unique_ptr<Command> cmd)
    {
        auto id = std::string(cmd->getIdentifier());
        auto nodePtr = std::make_unique<Node>(std::move(cmd));
        auto rawNodePtr = nodePtr.get();

        // used by commands in the tree to register subcommands
        nodePtr->command->setSubCommandCallback(std::make_unique<std::function<void(std::unique_ptr<Command>)>>(
            [rawNodePtr](std::unique_ptr<Command> subCommandPtr)
            {
                rawNodePtr->insertChild(std::move(subCommandPtr));
            }));

        children.try_emplace(id, std::move(nodePtr));
    }

    CommandTree::Node *CommandTree::Node::getChild(std::string_view id)
    {
        auto it = children.find(id);
        return (it != children.end()) ? it->second.get() : nullptr;
    }

    const CommandTree::Node *CommandTree::Node::getChild(std::string_view id) const
    {
        auto it = children.find(id);
        return (it != children.end()) ? it->second.get() : nullptr;
    }

    CommandTree::CommandTree()
    {
        root = std::make_unique<Node>(std::make_unique<Command>("root"));
        // used by commands in the tree to register subcommands
        root->command->setSubCommandCallback(std::make_unique<std::function<void(std::unique_ptr<Command>)>>(
            [this](std::unique_ptr<Command> subCommandPtr)
            {
                this->root->insertChild(std::move(subCommandPtr));
            }));
    };

    Command *CommandTree::findRecursive(const Node *node)
    {
        return node->command.get();
    }

    CommandTree::Node *CommandTree::findNodeRecursive(Node *node)
    {
        return node;
    }

    void CommandTree::print(std::ostream &os, int indent) const
    {
        printRecursive(os, root.get(), false, indent);
    }

    void CommandTree::printRecursive(std::ostream &os, const Node *node, bool last, int indentStep)
    {
        // Print indentation with vertical guides
        for (size_t i = 0; i < indentStep; ++i)
        {
            if (i + 1 == indentStep)
                os << (last? "`-- " : "|-- ");
            else
                os << "|   ";
        }

        os << node->command->getIdentifier() << "\n";

        // Iterate over children
        for (auto it = node->children.begin(); it != node->children.end(); ++it)
        {
            bool isLast = std::next(it) == node->children.end();
            printRecursive(os, it->second.get(), isLast, indentStep + 1);
        }
    }

    std::string CommandNotFoundException::buildMessage(const std::string &id, const std::vector<std::string> &chain)
    {
        std::ostringstream oss;
        oss << "Parent command not found: '" << id << "' in path [";
        for (size_t i = 0; i < chain.size(); ++i)
        {
            oss << chain[i];
            if (i + 1 < chain.size())
                oss << " -> ";
        }
        oss << "]";
        return oss.str();
    }
}