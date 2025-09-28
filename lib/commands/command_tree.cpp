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
        root = std::make_unique<Node>(std::make_unique<Command>("root", "Root command", "The root command", nullptr));
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
        for (auto const &[id, node] : root->getChildren())
            printRecursive(os, node.get(), indent);
    }

    void CommandTree::printRecursive(std::ostream &os, const Node *node, int indent)
    {
        os << std::string(indent, ' ') << "- " << node->command->getIdentifier() << "\n";
        for (auto &[_, child] : node->children)
            printRecursive(os, child.get(), indent + 2);
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