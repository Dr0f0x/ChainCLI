// Copyright 2025 Dominik Czekai
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

std::vector<Command *> CommandTree::getAllCommands() const
{
    std::vector<Command *> commands;
    if (root)
    {
        getAllCommandsRecursive(root.get(), commands);
    }
    return commands;
}

std::vector<const Command *> CommandTree::getAllCommandsConst() const
{
    std::vector<const Command *> commands;
    if (root)
    {
        getAllCommandsRecursive(root.get(), commands);
    }
    return commands;
}
} // namespace cli::commands