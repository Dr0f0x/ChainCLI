#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "cli_config.h"
#include "commands/command_tree.h"
#include "commands/docwriting/docwriting.h"
#include "logging/logger.h"
#include "parsing/parser.h"

#define RUN_CLI_APP(cliInstance, argc_, argv_)                                                     \
    try                                                                                            \
    {                                                                                              \
        return cliInstance.run(argc_, argv_);                                                      \
    }                                                                                              \
    catch (const std::exception &e)                                                                \
    {                                                                                              \
        cliInstance.Logger().error()                                                               \
            << "terminate called after throwing an instance of '" << typeid(e).name() << "'\n"     \
            << "  what(): " << e.what() << std::endl;                                              \
        std::abort();                                                                              \
    }

namespace cli
{
class CliBase
{
public:
    // Non-copyable
    CliBase(const CliBase &) = delete;
    CliBase &operator=(const CliBase &) = delete;

    CliBase(CliBase &&) = default;
    CliBase &operator=(CliBase &&) = default;

    explicit CliBase(std::string_view executableName);
    explicit CliBase(CliConfig &&config);
    ~CliBase() = default;

    commands::Command &createNewCommand(
        std::string_view id, std::unique_ptr<std::function<void(const CliContext &)>> actionPtr);

    commands::Command &createNewCommand(std::string_view id)
    {
        return createNewCommand(id, nullptr);
    };

    CliBase &withCommand(std::unique_ptr<commands::Command> subCommandPtr);

    CliBase &withCommand(commands::Command &&subCommand)
    {
        return withCommand(std::make_unique<commands::Command>(std::move(subCommand)));
    }

    [[nodiscard]] const commands::CommandTree &getCommandTree() const { return commandsTree; };

    [[nodiscard]] CliConfig &getConfig() { return *configuration; };

    void init();
    int run(int argc, char *argv[]);

    [[nodiscard]] logging::Logger &Logger() { return *logger; }

    void setLogger(std::unique_ptr<logging::Logger> &newLogger) { logger = std::move(newLogger); }

    void setLogger(std::unique_ptr<logging::Logger> &&newLogger) { logger = std::move(newLogger); }

private:
    int internalRun(std::span<char *const> args) const;
    const commands::Command *locateCommand(std::vector<std::string> &args) const;
    bool rootShortCircuits(std::vector<std::string> &args, const cli::commands::Command &cmd) const;
    bool commandShortCircuits(std::vector<std::string> &args,
                              const cli::commands::Command &cmd) const;
    void globalHelp() const;

    commands::CommandTree commandsTree;
    bool initialized{false};

    std::unique_ptr<logging::Logger> logger =
        std::make_unique<logging::Logger>(logging::LogLevel::DEBUG);

    std::unique_ptr<CliConfig> configuration;
    parsing::StringParser parser;
    cli::commands::docwriting::DocWriter docWriter;
};
} // namespace cli