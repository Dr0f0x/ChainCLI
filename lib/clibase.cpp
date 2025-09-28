#include "clibase.h"
#include "utils.h"
#include "logging/logger.h"
#include "commands/command.h"
#include "parsing/parser.h"
#include <iostream>

namespace cli
{
    void print(const std::string &msg)
    {
        std::cout << "[cli] " << msg << std::endl;
        parsing::test();
    }

    commands::Command &CliBase::newCommand(std::string_view id, std::string_view short_desc, std::string_view long_desc, std::unique_ptr<std::function<void()>> actionPtr)
    {
        auto cmd = std::make_unique<commands::Command>(id, short_desc, long_desc, std::move(actionPtr)); // default-constructed
        commands::Command &ref = *cmd;
        commandsTree.insert(std::move(cmd));
        return ref;
    }

    commands::Command &CliBase::newCommand(std::string_view id, std::unique_ptr<std::function<void()>> actionPtr)
    {
        return this->newCommand(id, "", "", std::move(actionPtr));
    }

    commands::Command &CliBase::newCommand(std::string_view id)
    {
        return this->newCommand(id, "", "", nullptr);
    }

    void CliBase::init()
    {
        // TODO should be done over a flag for the root command
        // newCommand("--help", "Show help", "Show help for all commands or a specific command", std::make_unique<std::function<void()>>([this](){ this->globalHelp(); }));
        commandsTree.forEachCommand(
            [](commands::Command &cmd)
            {
                cmd.buildDocStrings(); // no const_cast needed
            });
    }

    int CliBase::run(int argc, char *argv[]) const
    {
        try
        {
            return internalRun(argc, argv);
        }
        catch (const std::exception &e)
        {
            logger->error() << "terminate called after throwing an instance of '"
                      << typeid(e).name() << "'\n"
                      << "  what(): " << e.what() << std::endl;
            std::abort(); // simulate abnormal termination
        }
    }

    int CliBase::internalRun(int argc, char *argv[]) const
    {
        auto args = turnArgsToVector(argc, argv);

        for (const auto &arg : args)
            std::cout << arg << "\n";

        if (args.empty())
        {
            logger->trace("no command given"); // TODO call root command
            return 0;
        }

        if (const auto *cmd = locateCommand(args))
        {
            logger->info("Executing command: {}", cmd->getIdentifier());
            cmd->execute();
        }
        else
        {
            std::cout << "Unknown command: " << args[0] << "\n";
            globalHelp();
        }
        return 0;
    }

    commands::Command *CliBase::locateCommand(std::vector<std::string> const &args) const
    {
        const commands::CommandTree::Node *node = commandsTree.getRoot();

        commands::Command *cmd = nullptr;

        for (const auto &arg : args)
        {
            // Move one level down if child exists
            const auto *child = node->getChild(arg);
            if (!child)
            {
                return nullptr;
            }

            node = child;
            cmd = node->command.get();
        }
        return cmd;
    }

    void CliBase::globalHelp() const
    {
        logger->info() << "USAGE:\n\n";

        auto printCmd = [this](const commands::Command &cmd)
        {
            logger->info() << cmd.getDocStringShort() << "\n\n";
        };

        logger->info() << "Use --help <command> to get more information about a specific command" << std::endl;
    }
}