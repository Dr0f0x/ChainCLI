#include "clibase.h"
#include "logging/logger.h"
#include "commands/command.h"
#include <memory>
#include <iostream>

using namespace cli::logging;

int command_func(){
    std::cout << "command called" << std::endl;
    return 0;
    cli::CLI()->getLogger()->info("run command executed");
}

int main(int argc, char* argv[]) {
    cli::print("Hello from demo!");
    using namespace cli::logging;

    // Create a logger
    auto logger = cli::CLI()->getLogger();
    logger->setLevel(LogLevel::TRACE);

    // Attach console handler
    logger->addHandler(std::make_unique<ConsoleHandler>(std::make_unique<MessageOnlyFormatter>(), LogLevel::TRACE));

    // Attach file handler (logs everything to one file)
    logger->addHandler(std::make_unique<FileHandler>("app.log", std::make_unique<BasicFormatter>(), LogLevel::TRACE));

    // Log messages at different levels
    logger->trace("most detailed contains internal logs of the library");
    logger->verbose("very detailed information for inspection/development!");
    logger->debug("This is a debug message");
    logger->detail("more detailed than info!");
    logger->info("Application started");
    logger->warning("Low disk space warning");
    logger->error("Failed to open file");

    auto helpCmd = std::make_unique<cli::commands::Command>("run", "Run something", command_func);
    cli::CLI()->addCommand(std::move(helpCmd));

    if (auto cmd = cli::CLI()->getCommand("run")) {
        std::cout << *cmd << std::endl;
    }

    return cli::CLI()->run(argc, argv);
}