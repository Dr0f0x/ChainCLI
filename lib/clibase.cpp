#include "clibase.h"
#include "utils.h"
#include "logging/logger.h"
#include "commands/command.h"
#include "parsing/parser.h"
#include <iostream>

namespace cli {
    void print(const std::string& msg) {
        std::cout << "[cli] " << msg << std::endl;
        parsing::test();
    }

    void CliBase::addCommand(std::unique_ptr<commands::Command> cmd) {
        const auto id = cmd->getIdentifier();
        commandsMap.try_emplace(id, std::move(cmd));
    }

    commands::Command &CliBase::newCommand(std::string_view id, std::string_view short_desc, std::string_view long_desc, std::unique_ptr<std::function<void()>> actionPtr)
    {
        auto cmd = std::make_unique<commands::Command>(id, short_desc, long_desc, std::move(actionPtr)); // default-constructed
        commands::Command& ref = *cmd;
        commandsMap.try_emplace(id, std::move(cmd));
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

    commands::Command* CliBase::getCommand(std::string_view id) const {
        auto it = commandsMap.find(id);
        return (it != commandsMap.end()) ? it->second.get() : nullptr;
    }

    //TODO only for debug purposes remove later
    std::vector<commands::Command *> CliBase::getAllCommands() const
    {
        std::vector<commands::Command*> allCommands;
        allCommands.reserve(commandsMap.size());
        for (const auto& [id, cmdPtr] : commandsMap)
        {
            if (cmdPtr)
                allCommands.push_back(cmdPtr.get());
        }
        return allCommands;
    }

    int CliBase::run(int argc, char *argv[]) const
    {
        auto args = turnArgsToVector(argc, argv);

        for (const auto& arg : args)
            std::cout << arg << "\n";

        if (args.empty()){
            logger->trace("no command given");
            return 0;
        }

        if (auto cmd = getCommand(args[0])) {
            logger->trace("Executing command: {0}", *cmd);
            cmd->execute();
        } else {
            std::cout << "Unknown command: " << args[0] << "\n";
        }
        return 0;
    }
}