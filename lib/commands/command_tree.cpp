#include "command_tree.h"

#include <sstream>

namespace cli::commands
{
CommandTree::CommandTree(std::string_view rootName)
{
    root = std::make_unique<Command>(rootName);
}

std::string_view CommandTree::getPathForCommand(Command *cmd) const
{
    return commandPathMap.at(cmd);
}

void CommandTree::buildCommandPathMap(const std::string &separator)
{
    std::unordered_map<Command *, std::string> map;
    if (root)
    {
        std::vector<std::string> path;
        buildCommandPathMapRecursive(root.get(), path, separator);
    }
}

void CommandTree::buildCommandPathMapRecursive(Command *cmd, std::vector<std::string> &path,
                                               const std::string &separator)
{
    path.emplace_back(cmd->getIdentifier());

    // Build the full path string
    std::string fullPath;
    for (size_t i = 0; i < path.size(); ++i)
    {
        fullPath += path[i];
        if (i + 1 < path.size())
            fullPath += separator;
    }

    commandPathMap[cmd] = fullPath;

    // Recurse into subcommands
    for (const auto &[key, value] : cmd->getSubCommands())
    {
        buildCommandPathMapRecursive(value.get(), path, separator);
    }

    path.pop_back();
}

std::string CommandNotFoundException::buildMessage(const std::string &id,
                                                   const std::vector<std::string> &chain)
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
} // namespace cli::commands