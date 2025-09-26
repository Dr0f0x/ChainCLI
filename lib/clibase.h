#pragma once
#include "commands/Command.h"
#include "logging/logger.h"
#include <unordered_map>
#include <memory>
#include <string_view>
#include <string>

namespace cli
{
    void print(const std::string &msg);

    class CliBase
    {
    public:
        // Non-copyable
        CliBase(const CliBase &) = delete;
        CliBase &operator=(const CliBase &) = delete;

        CliBase(CliBase &&) = default;
        CliBase &operator=(CliBase &&) = default;

        CliBase() = default;
        ~CliBase() = default;

        void addCommand(std::unique_ptr<commands::Command> cmd);
        commands::Command *getCommand(std::string_view id) const;
        int run(int argc, char *argv[]) const;

        logging::ILogger *getLogger() { return logger.get(); }
        void setLogger(std::unique_ptr<logging::ILogger> newLogger) { logger = std::move(newLogger); }

    private:
        std::unordered_map<std::string_view, std::unique_ptr<commands::Command>> commandsMap;
        std::unique_ptr<logging::ILogger> logger = std::make_unique<logging::Logger>(logging::LogLevel::DEBUG);
    };

    inline std::unique_ptr<CliBase> const GlobalCli = std::make_unique<CliBase>();
    inline CliBase *CLI() { return GlobalCli.get(); }
} // namespace cli