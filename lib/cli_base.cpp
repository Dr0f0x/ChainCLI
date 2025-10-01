#include "cli_base.h"
#include "utils.h"
#include "logging/logger.h"
#include "commands/command.h"
#include "parsing/parser.h"
#include "cli_context.h"
#include <iostream>

namespace cli
{
    void print(const std::string &msg)
    {
        std::cout << "[cli] " << msg << std::endl;
    }

    CliBase::CliBase()
    {
        configuration = CliConfig();
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
        // TODO should be done over a flag for the root command
        // newCommand("--help", "Show help", "Show help for all commands or a specific command", std::make_unique<std::function<void()>>([this](){ this->globalHelp(); }));
        // configure root command

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

        if (args.empty())
        {
            logger->trace("no command given"); // TODO call root command
            return 0;
        }

        if (const auto *cmd = locateCommand(args))
        {
            logger->trace("Executing command: {}", cmd->getIdentifier());

            auto contextBuilder = cli::ContextBuilder();

            parsing::StringParser::parseArguments(cmd->getPositionalArguments(), cmd->getOptionArguments(), args, contextBuilder);
            cmd->execute(*contextBuilder.build());
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

    void CliBase::globalHelp() const
    {
        logger->info() << "USAGE:\n\n";

        auto printCmd = [this](const commands::Command &cmd)
        {
            logger->info() << cmd.getDocStringShort() << "\n\n";
        };

        commandsTree.forEachCommand(printCmd);

        logger->info() << "Use --help <command> to get more information about a specific command" << std::endl;
    }
}