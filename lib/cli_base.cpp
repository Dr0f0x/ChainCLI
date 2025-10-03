#include "cli_base.h"
#include "utils.h"
#include "logging/logger.h"
#include "commands/command.h"
#include "parsing/parser.h"
#include "cli_context.h"
#include <iostream>

namespace cli
{
    CliBase::CliBase(CliConfig &&config)
        : commandsTree(config.executableName)
    {
        configuration = std::make_unique<CliConfig>(std::move(config));
    }

    CliBase::CliBase(std::string_view executableName)
        : commandsTree(executableName)
    {
        configuration = std::make_unique<CliConfig>();
        configuration->executableName = std::string(executableName);
    }

    commands::Command &CliBase::createNewCommand(std::string_view id, std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
    {
        auto cmd = std::make_unique<commands::Command>(id, "", "", std::move(actionPtr)); // default-constructed
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

        commandsTree.forEachCommand(
            [](commands::Command &cmd)
            {
                cmd.buildDocStrings(); // no const_cast needed
            });
    }

    int CliBase::run(int argc, char *argv[])
    {
        if (!initialized)
        {
            init();
        }
        return internalRun(argc, argv);
    }

    int CliBase::internalRun(int argc, char *argv[]) const
    {
        auto args = turnArgsToVector(argc, argv);

        if (rootShortCircuits(args, *(commandsTree.getRootCommand())))
        {
            return 0;
        }

        if (const auto *cmd = locateCommand(args))
        {
            if(commandShortCircuits(args, *cmd)){
                return 0;
            }

            logger->trace("Executing command: {}", cmd->getIdentifier());

            auto contextBuilder = cli::ContextBuilder();

            parsing::StringParser::parseArguments(*cmd, args, contextBuilder);
            cmd->execute(*contextBuilder.build(*logger));
        }
        else
        {
            std::cout << "Unknown command: " << args[0] << "\n";
            globalHelp();
        }
        return 0;
    }

    // returns the found command and modifies args to only contain the values that werent consumed in the tree traversal
    const commands::Command *CliBase::locateCommand(std::vector<std::string> &args) const
    {
        const commands::Command *commandPtr = commandsTree.getRootCommand();

        size_t consumed = 0;

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

    bool CliBase::rootShortCircuits(std::vector<std::string> &args, const cli::commands::Command &cmd) const
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

    bool CliBase::commandShortCircuits(std::vector<std::string> &args, const cli::commands::Command& cmd) const
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

        auto printCmd = [this](const commands::Command &cmd)
        {
            if (cmd.hasExecutionFunction())
                logger->info() << cmd.getDocStringShort() << "\n\n";
        };

        commandsTree.forEachCommand(printCmd);

        logger->info() << "Use --help <command> to get more information about a specific command" << std::endl;
    }
}