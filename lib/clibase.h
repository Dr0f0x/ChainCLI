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
        commands::Command &newCommand(std::string_view id, std::string_view short_desc, std::string_view long_desc, std::unique_ptr<std::function<void()>> actionPtr);
        commands::Command &newCommand(std::string_view id, std::unique_ptr<std::function<void()>> actionPtr);
        commands::Command &newCommand(std::string_view id);

        commands::Command *getCommand(std::string_view id) const;
        std::vector<commands::Command*> getAllCommands() const;

        void init() const;
        int run(int argc, char *argv[]) const;

        logging::Logger &Logger() { return *logger; }
        void setLogger(std::unique_ptr<logging::Logger> &newLogger) { logger = std::move(newLogger); }
        void setLogger(std::unique_ptr<logging::Logger> &&newLogger) { logger = std::move(newLogger); }

    private:
        std::unordered_map<std::string_view, std::unique_ptr<commands::Command>> commandsMap;
        std::unique_ptr<logging::Logger> logger = std::make_unique<logging::Logger>(logging::LogLevel::DEBUG);
    };

    inline std::unique_ptr<CliBase> const GlobalCli = std::make_unique<CliBase>();
    inline CliBase &CLI()
    {
        return *GlobalCli;
    }
} // namespace cli