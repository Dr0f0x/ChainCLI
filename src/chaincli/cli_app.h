/*
 * Copyright 2025 Dominik Czekai
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
        cliInstance.Logger().error() << e.what() << std::endl;                                     \
        std::abort();                                                                              \
    }

namespace cli
{
/// @brief Main class representing a command-line application
class CliApp
{
public:
    // Non-copyable
    CliApp(const CliApp &) = delete;
    CliApp &operator=(const CliApp &) = delete;

    explicit CliApp(std::string_view executableName);
    explicit CliApp(CliConfig &&config);
    ~CliApp() = default;

    /// @brief Add a new command to the application
    /// @param subCommandPtr the unique pointer to the command to add
    /// @return a reference to this CliApp instance
    CliApp &withCommand(std::unique_ptr<commands::Command> subCommandPtr);

    /// @brief Add a new command to the application
    /// @param subCommand the command to add
    /// @return a reference to this CliApp instance
    CliApp &withCommand(commands::Command &&subCommand);

    /// @brief Initialize the CLI application, preparing it for execution
    /// This method sets up internal structures and should be called before `run()`
    /// if any commands have been added. If not called explicitly, it will be called
    /// automatically on the first invocation of `run()`.
    void init();

    /// @brief Run the CLI application with the given arguments
    /// @param argc the argument count
    /// @param argv the argument vector
    /// @return the exit code of the application
    int run(int argc, char *argv[]);

    /// @brief Get the logger instance used by the CLI application
    /// @return a reference to the logger instance
    [[nodiscard]] logging::Logger &Logger() { return *logger; }

    /// @brief Get the command tree used by the CLI application
    /// @return a reference to the command tree
    [[nodiscard]] const commands::CommandTree &getCommandTree() const { return commandsTree; };

    /// @brief Get the command tree used by the CLI application
    /// @return a reference to the command tree
    [[nodiscard]] commands::Command *getMainCommand() { return commandsTree.getRootCommand(); };

    /// @brief Get the configuration used by the CLI application
    /// @return a reference to the configuration
    [[nodiscard]] CliConfig &getConfig() { return *configuration; };

    /// @brief Get the documentation writer used by the CLI application
    /// @return a reference to the documentation writer
    [[nodiscard]] commands::docwriting::DocWriter &getDocWriter() { return docWriter; }

    /// @brief Set the logger instance used by the CLI application
    /// @param newLogger the new logger instance
    void setLogger(std::unique_ptr<logging::Logger> &&newLogger) { logger = std::move(newLogger); }

private:
    int internalRun(std::span<char *const> args);
    bool rootShortCircuits(std::vector<std::string> &args, const cli::commands::Command &cmd) const;
    bool commandShortCircuits(std::vector<std::string> &args, cli::commands::Command *cmd) const;

    commands::CommandTree commandsTree;
    bool initialized{false};

    std::unique_ptr<logging::Logger> logger =
        std::make_unique<logging::Logger>(logging::LogLevel::DEBUG);

    std::unique_ptr<CliConfig> configuration;
    parsing::Parser parser;
    cli::commands::docwriting::DocWriter docWriter;
};
} // namespace cli