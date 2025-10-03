#include "command_tree.h"
#include <sstream>

namespace cli::commands
{
    CommandTree::CommandTree(std::string_view rootName)
    {
        root = std::make_unique<Command>(rootName);
    }

    void CommandTree::print(std::ostream &os, int indent) const
    {
        printRecursive(os, root.get(), false, indent);
    }

    void CommandTree::printRecursive(std::ostream &os, const Command *cmdPtr, bool last, int indentStep)
    {
        // Print indentation with vertical guides
        for (size_t i = 0; i < indentStep; ++i)
        {
            if (i + 1 == indentStep)
                os << (last? "`-- " : "|-- ");
            else
                os << "|   ";
        }

        os << cmdPtr->getIdentifier() << "\n";

        // Iterate over children
        for (auto it = cmdPtr->getSubCommands().begin(); it != cmdPtr->getSubCommands().end(); ++it)
        {
            bool isLast = std::next(it) == cmdPtr->getSubCommands().end();
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