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

#include "cli_app.h"

#include <iostream>

#include "cli_context.h"
#include "commands/command.h"
#include "context_builder.h"
#include "logging/logger.h"

#define inline_t

namespace cli
{
CliApp::CliApp(CliConfig &&config)
    : commandsTree(config.executableName),
      configuration(std::make_unique<CliConfig>(std::move(config))), parser(*configuration),
      docWriter(*configuration)
{
}

CliApp::CliApp(std::string_view executableName)
    : commandsTree(executableName), configuration(std::make_unique<CliConfig>()),
      parser(*configuration), docWriter(*configuration)
{
    configuration->executableName = std::string(executableName);
}

CliApp &CliApp::withCommand(std::unique_ptr<commands::Command> subCommandPtr)
{
    commandsTree.insert(std::move(subCommandPtr));
    return *this;
}

void CliApp::init()
{
    initialized = true;

    commandsTree.buildCommandPathMap();

    commandsTree.forEachCommand([this](commands::Command *cmd) {
        docWriter.setDocStrings(*cmd, commandsTree.getPathForCommand(cmd));
    });
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
int CliApp::run(int argc, char *argv[])
{
    if (!initialized)
    {
        init();
    }
    return internalRun(std::span<char *const>(argv + 1, argc - 1));
}

// returns the found command and modifies args to only contain the values that
// werent consumed in the tree traversal
inline_t commands::Command *locateCommand(commands::CommandTree &commandsTree, std::vector<std::string> &args)
{
    commands::Command *commandPtr = commandsTree.getRootCommand();

    int consumed = 0;

    for (const auto &arg : args)
    {
        // Move one level down if child exists
        commands::Command *subCommandPtr = commandPtr->getSubCommand(arg);
        if (!subCommandPtr)
        {
            break;
        }

        commandPtr = subCommandPtr;
        ++consumed;
    }
    args.erase(args.begin(), args.begin() + consumed);
    return commandPtr;
}

int CliApp::internalRun(std::span<char *const> args)
{
    std::vector<std::string> argVec(args.begin(), args.end());

    if (rootShortCircuits(argVec, *(commandsTree.getRootCommand())))
    {
        return 0;
    }

    if (commands::Command *cmd = locateCommand(commandsTree, argVec);
        cmd && cmd->hasExecutionFunction())
    {
        if (commandShortCircuits(argVec, cmd))
        {
            return 0;
        }

        logger->trace("Executing command: {}", cmd->getIdentifier());

        auto contextBuilder = cli::ContextBuilder();

        parser.parseArguments(*cmd, argVec, contextBuilder);
        cmd->execute(*contextBuilder.build(*logger));
    }
    else
    {
        std::cout << "Unknown command: " << args[0] << "\n";
        auto allCommands = commandsTree.getAllCommandsConst();
        logger->info(docWriter.generateAppDocString(allCommands));
    }
    return 0;
}

bool CliApp::rootShortCircuits(std::vector<std::string> &args,
                               const cli::commands::Command &cmd) const
{
    if (args.empty() && !cmd.hasExecutionFunction())
    {
        auto allCommands = commandsTree.getAllCommandsConst();
        logger->info(docWriter.generateAppDocString(allCommands));
        return true;
    }

    if (args.size() == 1)
    {
        if (args.at(0) == "-h" || args.at(0) == "--help")
        {
            auto allCommands = commandsTree.getAllCommandsConst();
            logger->info(docWriter.generateAppDocString(allCommands));
            return true;
        }
        else if (args.at(0) == "-v" || args.at(0) == "--version")
        {
            logger->info(docWriter.generateAppVersionString());
            return true;
        }
    }
    return false;
}

bool CliApp::commandShortCircuits(std::vector<std::string> &args,
                                  cli::commands::Command *cmd) const
{
    if (args.size() == 1 && (args.at(0) == "-h" || args.at(0) == "--help"))
    {
        logger->info(std::string(docWriter.generateCommandDocString(*cmd)));
        return true;
    }
    return false;
}

CliApp &CliApp::withCommand(commands::Command &&subCommand)
{
    return withCommand(std::make_unique<commands::Command>(std::move(subCommand)));
}

} // namespace cli