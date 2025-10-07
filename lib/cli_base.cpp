#include "cli_base.h"

#include <iostream>

#include "cli_context.h"
#include "commands/command.h"
#include "logging/logger.h"

namespace cli
{
CliBase::CliBase(CliConfig &&config)
    : commandsTree(config.executableName),
      configuration(std::make_unique<CliConfig>(std::move(config))), parser(*configuration),
      docWriter(*configuration)
{
}

CliBase::CliBase(std::string_view executableName)
    : commandsTree(executableName), configuration(std::make_unique<CliConfig>()),
      parser(*configuration), docWriter(*configuration)
{
    configuration->executableName = std::string(executableName);
}

commands::Command &CliBase::createNewCommand(
    std::string_view id, std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
{
    auto cmd = std::make_unique<commands::Command>(id, "", "",
                                                   std::move(actionPtr)); // default-constructed
    commands::Command &ref = *cmd;
    commandsTree.insert(std::move(cmd));
    return ref;
}

CliBase &CliBase::withCommand(std::unique_ptr<commands::Command> subCommandPtr)
{
    commandsTree.insert(std::move(subCommandPtr));
    return *this;
}

void CliBase::init()
{
    initialized = true;

    commandsTree.buildCommandPathMap();

    commandsTree.forEachCommand([this](commands::Command *cmd) {
        docWriter.setDocStrings(*cmd, commandsTree.getPathForCommand(cmd));
    });
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
int CliBase::run(int argc, char *argv[])
{
    if (!initialized)
    {
        init();
    }
    return internalRun(std::span<char *const>(argv + 1, argc - 1));
}

int CliBase::internalRun(std::span<char *const> args) const
{
    std::vector<std::string> argVec(args.begin(), args.end());

    if (rootShortCircuits(argVec, *(commandsTree.getRootCommand())))
    {
        return 0;
    }

    if (const auto *cmd = locateCommand(argVec))
    {
        if (commandShortCircuits(argVec, *cmd))
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
        globalHelp();
    }
    return 0;
}

// returns the found command and modifies args to only contain the values that
// werent consumed in the tree traversal
const commands::Command *CliBase::locateCommand(std::vector<std::string> &args) const
{
    const commands::Command *commandPtr = commandsTree.getRootCommand();

    int consumed = 0;

    for (const auto &arg : args)
    {
        // Move one level down if child exists
        const auto *subCommandPtr = commandPtr->getSubCommand(arg);
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

bool CliBase::rootShortCircuits(std::vector<std::string> &args,
                                const cli::commands::Command &cmd) const
{
    if (args.empty() && !cmd.hasExecutionFunction())
    {
        globalHelp();
        return true;
    }

    if (args.size() == 1)
    {
        if (args.at(0) == "-h" || args.at(0) == "--help")
        {
            globalHelp();
            return true;
        }
        else if (args.at(0) == "-v" || args.at(0) == "--version")
        {
            logger->info("Version: {}", configuration->version);
            return true;
        }
    }
    return false;
}

bool CliBase::commandShortCircuits(std::vector<std::string> &args,
                                   const cli::commands::Command &cmd) const
{
    if (args.size() == 1 && (args.at(0) == "-h" || args.at(0) == "--help"))
    {
        logger->info(std::string(cmd.getDocStringLong()));
        return true;
    }
    return false;
}

void CliBase::globalHelp() const
{
    logger->info() << configuration->description << "\n\n";

    auto printCmd = [this](const commands::Command *cmd) {
        if (cmd->hasExecutionFunction())
            logger->info() << cmd->getDocStringShort() << "\n\n";
    };

    commandsTree.forEachCommand(printCmd);

    logger->info() << "Use --help <command> to get more information about a specific command"
                   << std::endl;
}
} // namespace cli