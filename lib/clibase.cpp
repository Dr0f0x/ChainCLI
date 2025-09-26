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

    commands::Command* CliBase::getCommand(std::string_view id) const {
        auto it = commandsMap.find(id);
        return (it != commandsMap.end()) ? it->second.get() : nullptr;
    }

    int CliBase::run(int argc, char *argv[]) const
    {
        auto args = turnArgsToVector(argc, argv);

        for (const auto& arg : args)
            std::cout << arg << "\n";

        if (auto cmd = getCommand(args[0])) {
            logger->trace("Executing command: {0}", *cmd);
            cmd->execute();
        } else {
            std::cout << "Unknown command: " << args[0] << "\n";
        }
        return 1;
    }
}