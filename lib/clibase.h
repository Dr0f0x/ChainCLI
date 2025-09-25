#pragma once
#include "commands\Command.h"
#include <unordered_map>
#include <memory>
#include <string_view>
#include <string>

namespace cli {
    void print(const std::string& msg);

    class CliBase {
    public:
        CliBase() = default;
        ~CliBase() = default;

        void addCommand(std::unique_ptr<commands::Command> cmd);
        commands::Command* getCommand(std::string_view id) const;
        void run(int argc, char* argv[]) const;

    private:
        std::unordered_map<std::string_view, std::unique_ptr<commands::Command>> commandsMap;
    };

} // namespace cli