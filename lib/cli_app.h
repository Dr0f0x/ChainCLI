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
class CliApp
{
public:
    // Non-copyable
    CliApp(const CliApp &) = delete;
    CliApp &operator=(const CliApp &) = delete;

    explicit CliApp(std::string_view executableName);
    explicit CliApp(CliConfig &&config);
    ~CliApp() = default;

    commands::Command &createNewCommand(
        std::string_view id, std::unique_ptr<std::function<void(const CliContext &)>> actionPtr);

    commands::Command &createNewCommand(std::string_view id)
    {
        return createNewCommand(id, nullptr);
    };

    CliApp &withCommand(std::unique_ptr<commands::Command> subCommandPtr);

    CliApp &withCommand(commands::Command &&subCommand)
    {
        return withCommand(std::make_unique<commands::Command>(std::move(subCommand)));
    }

    void init();
    int run(int argc, char *argv[]);

    [[nodiscard]] logging::Logger &Logger() { return *logger; }

    [[nodiscard]] const commands::CommandTree &getCommandTree() const { return commandsTree; };

    [[nodiscard]] CliConfig &getConfig() { return *configuration; };

    [[nodiscard]] commands::docwriting::DocWriter &getDocWriter() { return docWriter; }

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