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
        parsing::test();
    }

    CliBase::CliBase()
    {
        configuration = CliConfig();
    }

    commands::Command &CliBase::newCommand(std::string_view id, std::string_view short_desc, std::string_view long_desc, std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
    {
        auto cmd = std::make_unique<commands::Command>(id, short_desc, long_desc, std::move(actionPtr)); // default-constructed
        commands::Command &ref = *cmd;
        commandsTree.insert(std::move(cmd));
        return ref;
    }

    commands::Command &CliBase::newCommand(std::string_view id, std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
    {
        return this->newCommand(id, "", "", std::move(actionPtr));
    }

    commands::Command &CliBase::newCommand(std::string_view id)
    {
        return this->newCommand(id, "", "", nullptr);
    }

    void CliBase::init()
    {
        initialized = true;
        // TODO should be done over a flag for the root command
        // newCommand("--help", "Show help", "Show help for all commands or a specific command", std::make_unique<std::function<void()>>([this](){ this->globalHelp(); }));
        // configure root command
        auto root = commandsTree.getRoot();
        root->command = std::make_unique<cli::commands::Command>(configuration.executableName);

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

            auto parsed = parsing::StringParser::parsePositionalArguments(cmd->getArguments(), args, contextBuilder);
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
    commands::Command *CliBase::locateCommand(std::vector<std::string> &args) const
    {
        const commands::CommandTree::Node *node = commandsTree.getRoot();

        commands::Command *cmd = nullptr;
        size_t consumed = 0;

        for (const auto &arg : args)
        {
            // Move one level down if child exists
            const auto *child = node->getChild(arg);
            if (!child)
            {
                break;
            }

            node = child;
            cmd = node->command.get();
            ++consumed;
        }
        args.erase(args.begin(), args.begin() + consumed);
        return cmd;
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