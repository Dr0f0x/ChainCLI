#include "clibase.h"
#include "utils.h"
#include "logging\logger.h"
#include "commands\command.h"
#include "parsing\parser.h"
#include <iostream>

namespace cli {
    void print(const std::string& msg) {
        std::cout << "[cli] " << msg << std::endl;
        logging::test();
        commands::test();
        parsing::test();
    }


    void CliBase::addCommand(std::unique_ptr<commands::Command> cmd) {
        const auto id = cmd->getIdentifier();
        commandsMap.emplace(id, std::move(cmd));
    }

    commands::Command* CliBase::getCommand(std::string_view id) const {
        auto it = commandsMap.find(id);
        return (it != commandsMap.end()) ? it->second.get() : nullptr;
    }

    int CliBase::run(int argc, char *argv[]) const
    {
        auto args = turnArgsToVector(argc, argv);

        //TODO turn into logging
        for (const auto& arg : args)
            std::cout << arg << "\n";

        //no flags passed
        if (args.size() == 0){
            std::cout << "no command given " << "\n";
            return 0;
        }

        auto cmd = getCommand(args[0]);
        if (cmd) {
            std::cout << "Executing command: " << *cmd << "\n";
            cmd->execute();
        } else {
            std::cout << "Unknown command: " << args[0] << "\n";
        }
        return 1;
    }
}