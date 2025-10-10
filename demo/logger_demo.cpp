#include "chain_cli.hpp"

int main(int argc, char *argv[])
{
    auto config = cli::CliConfig();
    config.title = "Logger Demo";
    config.executableName = "logger_demo";
    config.description = "Logger Demo to test the CLI Library";
    config.version = "1.0.0";

    auto cliApp = cli::CliApp(std::move(config));

    // Set logger level to TRACE to see all messages
    cliApp.Logger().setLevel(cli::logging::LogLevel::TRACE);

    // Log messages at different levels

    //using streams, have to be manually flushed and the full buffer is emitted as one log record
    cliApp.Logger().trace() << "This is a TRACE message" << std::flush;
    cliApp.Logger().verbose() << "This is a VERBOSE message" << std::flush;
    cliApp.Logger().debug() << "This is a DEBUG message" << std::flush;
    cliApp.Logger().info() << "This is an INFO message" << std::flush;
    cliApp.Logger().warning() << "This is a WARNING message" << std::flush;
    cliApp.Logger().success() << "This is a SUCCESS message" << std::flush;
    cliApp.Logger().error() << "This is an ERROR message" << std::flush;

    //using format strings, each call to the method emits one log record
    cliApp.Logger().trace("This is a TRACE message with number: {}", 1);
    cliApp.Logger().info("Logging demo completed successfully.");

    constexpr int debugX = 42;
    constexpr double debugY = 3.54;
    cliApp.Logger().debug("Debug details: x = {}, y = {}", debugX, debugY);

    cliApp.Logger().error("An error occurred: {}", "File not found");
    cliApp.Logger().success("Logging demo completed successfully.");

    RUN_CLI_APP(cliApp, argc, argv);
}