#pragma once
#include "commands/command_tree.h"
#include "logging/logger.h"
#include <unordered_map>
#include <memory>
#include <string_view>
#include <string>
#include "cli_config.h"

#define RUN_WITH_TRY_CATCH(call)                                                          \
    try                                                                                   \
    {                                                                                     \
        return call;                                                                      \
    }                                                                                     \
    catch (const std::exception &e)                                                       \
    {                                                                                     \
        cli::CLI().Logger().error() << "terminate called after throwing an instance of '" \
                                    << typeid(e).name() << "'\n"                          \
                                    << "  what(): " << e.what() << std::endl;             \
        std::abort();                                                                     \
    }

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

        CliBase();
        ~CliBase() = default;

        commands::Command &newCommand(std::string_view id, std::string_view short_desc, std::string_view long_desc, std::unique_ptr<std::function<void(const CliContext &)>> actionPtr);
        commands::Command &newCommand(std::string_view id, std::unique_ptr<std::function<void(const CliContext &)>> actionPtr);
        commands::Command &newCommand(std::string_view id);

        [[nodiscard]] const commands::CommandTree &getCommandTree() const { return commandsTree; };
        [[nodiscard]] CliConfig &getConfig() { return configuration; };

        int run(int argc, char *argv[]);

        [[nodiscard]] logging::Logger &Logger() { return *logger; }
        void setLogger(std::unique_ptr<logging::Logger> &newLogger) { logger = std::move(newLogger); }
        void setLogger(std::unique_ptr<logging::Logger> &&newLogger) { logger = std::move(newLogger); }

    private:
        void init();
        int internalRun(int argc, char *argv[]) const;
        commands::Command *locateCommand(std::vector<std::string> &args) const;
        void globalHelp() const;

        commands::CommandTree commandsTree;
        bool initialized{false};

        std::unique_ptr<logging::Logger> logger = std::make_unique<logging::Logger>(logging::LogLevel::DEBUG);
        CliConfig configuration;
    };

    inline std::unique_ptr<CliBase> const GlobalCli = std::make_unique<CliBase>();
    inline CliBase &CLI()
    {
        return *GlobalCli;
    }
} // namespace cli