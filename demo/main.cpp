#include "cli_base.h"
#include "logging/logger.h"
#include "commands/command.h"
#include <memory>
#include <iostream>
#include <commands/command_tree.h>

using namespace cli::logging;

void command_func()
{
    std::cout << "command called" << std::endl;
    cli::CLI().Logger().info("run command executed");
}

void other_func()
{
    std::cout << "other command called" << std::endl;
    cli::CLI().Logger().info("other command executed");
}

void exception_func()
{
    std::cout << "exception command called" << std::endl;
    throw std::runtime_error("error");
}

void initCommands()
{
    // use newCommand helper
    cli::CLI().newCommand("other", "other short", "other long", std::make_unique<std::function<void()>>(other_func));

    // use step by step new commands
    cli::CLI().newCommand("run2", std::make_unique<std::function<void()>>(other_func))
        .withShortDescription("run2 short")
        .withLongDescription("run2 long");

    auto arg2 = cli::commands::PositionalArgument("arg2", typeid(int));
    arg2.withShortName("-a2")
        .withUsageComment("second argument")
        .withRequired(false);

    auto testcmdSub = cli::commands::Command("subchild1");
    testcmdSub.withShortDescription("Subchild 1")
    .withLongDescription("First subchild command")
    .withExecutionFunc(std::function<void()>(exception_func));

    auto& otherCmd = cli::CLI().newCommand("other2")
        .withShortDescription("other2 short")
        .withLongDescription("other2 long")
        .withExecutionFunc(std::function<void()>(exception_func))
        .withArgument(cli::commands::newArgument<int>("arg1", "-a1", "first argument", true))
        .withArgument(arg2);

    otherCmd.withSubCommand(testcmdSub);

    auto testcmd = cli::commands::Command("testchild2", "testSubchild 2", "Second subchild command", nullptr);
    otherCmd.withSubCommand(testcmd);
    //testcmd.withSubCommand(testcmdSub);
}

void configureCLI(){
    auto& config = cli::CLI().getConfig();
    config.executableName = "cliLibDemo";
}

void printCommands()
{
    std::cout << "Available commands:\n";

    auto &commandsTree = cli::CLI().getCommandTree();

    commandsTree.forEachCommand(
        [](cli::commands::Command const &cmd)
        {
            std::cout << "  " << cmd << "\n"; // relies on Command::operator<<
            std::cout << "---------\n";
            std::cout << cmd.getDocStringLong() << "\n";
            std::cout << "---------\n";
            std::cout << cmd.getDocStringShort() << "\n\n";
        });
    commandsTree.print(cli::CLI().Logger().info());
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
    auto cmdTree = cli::commands::CommandTree();

    // Insert commands into the tree
    cmdTree.insert(std::make_unique<cli::commands::Command>("child1", "Child 1", "First child command", nullptr));
    cmdTree.insert(std::make_unique<cli::commands::Command>("child2", "Child 2", "Second child command", nullptr));
    cmdTree.insert(std::make_unique<cli::commands::Command>("subchild1", "Subchild 1", "First subchild command", nullptr), "child1");

    cmdTree.print(std::cout);
}

int main(int argc, char *argv[])
{
    using namespace cli::logging;

    // setup logger
    auto &logger = cli::CLI().Logger();
    logger.setLevel(LogLevel::TRACE);

    auto consoleHandlerPtr = std::make_unique<ConsoleHandler>(std::make_shared<MessageOnlyFormatter>(), LogLevel::TRACE);
    // Attach console handler
    logger.addHandler(std::move(consoleHandlerPtr));
    // Attach file handler (logs everything to one file)
    logger.addHandler(std::make_unique<FileHandler>("app.log", std::make_shared<BasicFormatter>(), LogLevel::TRACE));

    logTest(logger);

    initCommands();
    configureCLI();

    cli::CLI().init();

    printCommands();

    return cli::CLI().run(argc, argv);
}