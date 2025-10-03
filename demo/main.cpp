#include "cli_base.h"
#include "logging/logger.h"
#include "commands/command.h"
#include <memory>
#include <iostream>
#include <commands/command_tree.h>

using namespace cli::logging;

void command_func(const cli::CliContext &ctx)
{
    std::cout << "command called" << std::endl;
    ctx.Logger.info("run command executed");
}

void other_func(const cli::CliContext &ctx)
{
    std::cout << "other command called" << std::endl;
    ctx.Logger.info("other command executed");
}

void exception_func(const cli::CliContext &ctx)
{
    int arg1 = ctx.getPositionalArgument<int>("arg1");
    std::string arg2;
    ctx.getPositionalArgument("arg2", arg2);

    auto pres = ctx.getOptionArgument<int>("--type");
    bool flag = ctx.isFlagPresent("--help");
    std::cout << "exception command called" << std::endl;
    throw std::runtime_error("error");
}

void initCommands(cli::CliBase &cliApp)
{
    // use newCommand helper
    cliApp.createNewCommand("other", std::make_unique<std::function<void(const cli::CliContext &)>>(other_func))
        .withLongDescription("other long")
        .withShortDescription("other short");

    // use step by step new commands
    cliApp.createNewCommand("run2", std::make_unique<std::function<void(const cli::CliContext &)>>(other_func))
        .withShortDescription("run2 short")
        .withLongDescription("run2 long");

    auto arg2 = cli::commands::PositionalArgument<std::string>("arg2");
    arg2.withOptionsComment("second argument")
        .withRequired(true);

    auto testcmdSub = cli::commands::Command("subchild1");
    testcmdSub.withShortDescription("Subchild 1")
        .withLongDescription("First subchild command")
        .withExecutionFunc(exception_func);

    auto otherCmd = cli::commands::Command("other2");
    otherCmd.withShortDescription("other2 short")
        .withLongDescription("other2 long")
        .withExecutionFunc(std::function<void(const cli::CliContext &)>(exception_func))
        .withExclusiveGroup(cli::commands::PositionalArgument<int>("arg1", "first argument", true),
                            std::move(arg2))
        .withOptionArgument(cli::commands::OptionArgument<int>("--type", "nut", "-t"))
        .withFlagArgument(cli::commands::FlagArgument("--help", "-h"));

    otherCmd.withSubCommand(std::move(testcmdSub));

    auto testcmd = cli::commands::Command("testchild2", "testSubchild 2", "Second subchild command", nullptr);
    otherCmd.withSubCommand(std::move(testcmd));

    cliApp.withCommand(std::move(otherCmd));
}

void configureCLI(cli::CliBase &cliApp)
{
    auto &config = cliApp.getConfig();
    config.executableName = "cliLibDemo";
}

void printCommands(cli::CliBase &cliApp)
{
    std::cout << "Available commands:\n";

    auto &commandsTree = cliApp.getCommandTree();

    commandsTree.forEachCommand(
        [](cli::commands::Command const &cmd)
        {
            std::cout << "  " << cmd << "\n"; // relies on Command::operator<<
            std::cout << "---------\n";
            std::cout << cmd.getDocStringLong() << "\n";
            std::cout << "---------\n";
            std::cout << cmd.getDocStringShort() << "\n\n";
        });
    commandsTree.print(cliApp.Logger().info());
    std::cout << "\n";
}

void logTest(Logger &logger)
{
    // Log messages at different levels
    logger.trace("most detailed contains internal logs of the library");
    logger.verbose("very detailed information for inspection/development!");
    logger.debug("This is a debug message");
    logger.success("more detailed than info!");
    logger.info("Application started");
    logger.warning("Low disk space warning");
    logger.error("Failed to open file");
}

void CommandTreeTest()
{
    // Create a command tree
    auto cmdTree = cli::commands::CommandTree("CLIDemo");

    // Insert commands into the tree
    cmdTree.insert(std::make_unique<cli::commands::Command>("child1", "Child 1", "First child command", nullptr));
    cmdTree.insert(std::make_unique<cli::commands::Command>("child2", "Child 2", "Second child command", nullptr));
    cmdTree.insert(std::make_unique<cli::commands::Command>("subchild1", "Subchild 1", "First subchild command", nullptr), "child1");

    cmdTree.print(std::cout);
}

int main(int argc, char *argv[])
{
    using namespace cli::logging;

    auto config = cli::CliConfig();
    config.executableName = "CLIDemo";
    config.description = "Demo to test the CLI Library";
    config.version = "1.0.0";

    auto cliApp = cli::CliBase(std::move(config));
    auto &logger = cliApp.Logger();
    logger.setLevel(LogLevel::TRACE);

    auto consoleHandlerPtr = std::make_unique<ConsoleHandler>(std::make_shared<MessageOnlyFormatter>(), LogLevel::TRACE);
    // Attach console handler
    logger.addHandler(std::move(consoleHandlerPtr));
    // Attach file handler (logs everything to one file)
    logger.addHandler(std::make_unique<FileHandler>("app.log", std::make_shared<BasicFormatter>(), LogLevel::TRACE));

    //logTest(logger);

    initCommands(cliApp);
    configureCLI(cliApp);
    // printCommands();
    RUN_CLI_APP(cliApp, argc, argv);
}