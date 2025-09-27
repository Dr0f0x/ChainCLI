#include "clibase.h"
#include "logging/logger.h"
#include "commands/command.h"
#include <memory>
#include <iostream>

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

void initCommands()
{
    //create own command
    std::string text = "Run something";
    auto helpCmd = std::make_unique<cli::commands::Command>("run", text, "Run something long", std::make_unique<std::function<void()>>(command_func));
    cli::CLI().addCommand(std::move(helpCmd));

    //use newCommand helper
    cli::CLI().newCommand("other", "other short", "other long", std::make_unique<std::function<void()>>(other_func));

    //use step by step new commands
    cli::CLI().newCommand("run2", std::make_unique<std::function<void()>>(other_func))
        .withShortDescription("run2 short")
        .withLongDescription("run2 long");

    cli::CLI().newCommand("other2")
        .withShortDescription("run2 short")
        .withLongDescription("run2 long")
        .withExecutionFunc(std::make_unique<std::function<void()>>(other_func));
        
    std::cout << "Available commands:\n";
    for (const auto& cmdPtr : cli::CLI().getAllCommands()) {
        if (cmdPtr) {
            std::cout << "  " << *cmdPtr << "\n"; // relies on Command::operator<<
        }
    }
}

int main(int argc, char *argv[])
{
    cli::print("Hello from demo!");
    using namespace cli::logging;

    // Create a logger
    auto& logger = cli::CLI().Logger();
    logger.setLevel(LogLevel::TRACE);

    auto consoleHandlerPtr = std::make_unique<ConsoleHandler>(std::make_shared<MessageOnlyFormatter>(), LogLevel::TRACE);
    // Attach console handler
    logger.addHandler(std::move(consoleHandlerPtr));

    // Attach file handler (logs everything to one file)
    logger.addHandler(std::make_unique<FileHandler>("app.log", std::make_shared<BasicFormatter>(), LogLevel::TRACE));

    // Log messages at different levels
    logger.trace("most detailed contains internal logs of the library");
    logger.verbose("very detailed information for inspection/development!");
    logger.debug("This is a debug message");
    logger.detail("more detailed than info!");
    logger.info("Application started");
    logger.warning("Low disk space warning");
    logger.error("Failed to open file");

    initCommands();

    return cli::CLI().run(argc, argv);
}