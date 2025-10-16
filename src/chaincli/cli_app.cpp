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
      configuration(std::make_unique<CliConfig>(std::move(config))),
      logger(std::make_unique<logging::Logger>()),
      parser(*configuration),
      docWriter(*configuration)
{
}

CliApp::CliApp(std::string_view executableName)
    : commandsTree(executableName),
      configuration(std::make_unique<CliConfig>()),
      logger(std::make_unique<logging::Logger>()),
      parser(*configuration),
      docWriter(*configuration)
{
    configuration->executableName = std::string(executableName);
}

CliApp::CliApp(const CliConfig &config, std::unique_ptr<logging::AbstractLogger> logger)
    : commandsTree(config.executableName),
      configuration(std::make_unique<CliConfig>(config)),
      logger(std::move(logger)),
      parser(*configuration),
      docWriter(*configuration)
{
}

CliApp &CliApp::withCommand(std::unique_ptr<commands::Command> subCommandPtr)
{
    commandsTree.insert(std::move(subCommandPtr));
    return *this;
}

void CliApp::init()
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Initializing CLI application...\n";
#endif
    initialized = true;

    commandsTree.buildCommandPathMap();

#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Building documentation strings for commands...\n";
#endif
    commandsTree.forEachCommand([this](commands::Command *cmd) {
        docWriter.setDocStrings(*cmd, commandsTree.getPathForCommand(cmd));
    });
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
int CliApp::run(int argc, char *argv[])
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Starting CLI application with " << argc << " arguments\n";
    std::cout << "Arguments: ";
    for (int i = 0; i < argc; ++i) {
        std::cout << "\"" << argv[i] << "\" ";
    }
    std::cout << "\n";
#endif
    if (!initialized)
    {
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Application not initialized, initializing now...\n";
#endif
        init();
    }
    return internalRun(std::span<char *const>(argv + 1, argc - 1));
}

// returns the found command and modifies args to only contain the values that
// werent consumed in the tree traversal
inline_t commands::Command *locateCommand(commands::CommandTree &commandsTree, std::vector<std::string> &args)
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Locating command in tree with arguments: ";
    for (const auto &arg : args) {
        std::cout << "\"" << arg << "\" ";
    }
    std::cout << "\n";
#endif
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
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Located command: " << commandPtr->getIdentifier() << ", consumed " << consumed << " arguments\n";
    std::cout << "Remaining arguments: ";
    for (const auto &arg : args) {
        std::cout << "\"" << arg << "\" ";
    }
    std::cout << "\n";
#endif
    return commandPtr;
}

int CliApp::internalRun(std::span<char *const> args)
{
    std::vector<std::string> argVec(args.begin(), args.end());

    if (rootShortCircuits(argVec, *(commandsTree.getRootCommand())))
    {
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Root short-circuit triggered, exiting early\n";
#endif
        return 0;
    }

    if (const commands::Command *cmd = locateCommand(commandsTree, argVec);
        cmd && cmd->hasExecutionFunction())
    {
        if (commandShortCircuits(argVec, cmd))
        {
#ifdef CHAIN_CLI_VERBOSE
            std::cout << "Command short-circuit triggered for: " << cmd->getIdentifier() << "\n";
#endif
            return 0;
        }

        #ifdef CHAIN_CLI_VERBOSE
        std::cout << "Executing command: " << cmd->getIdentifier() << "\n";
        #endif

        auto contextBuilder = cli::ContextBuilder();

#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Parsing arguments for command execution...\n";
#endif
        parser.parseArguments(*cmd, argVec, contextBuilder);
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Building context and executing command...\n";
#endif
        cmd->execute(*contextBuilder.build(*logger));
    }
    else
    {
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "No valid command found or command has no execution function\n";
#endif
        logger->error() << "Unknown command: " << args[0] << "\n" << std::flush;
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
                                  const cli::commands::Command *cmd) const
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