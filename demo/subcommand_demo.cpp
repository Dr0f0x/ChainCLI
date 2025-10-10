#include "chain_cli.hpp"

void initCommands(cli::CliApp &app)
{
    auto parentCmd = cli::commands::Command("parent");
    parentCmd.withShortDescription("Parent command")
        .withShortDescription("Parent command with subcommands")
        .withLongDescription("A parent command that has subcommands, but itself has no execute "
                             "function and therefore doesnt get shown in the help message");

    auto child1Cmd = cli::commands::Command("child1");
    child1Cmd.withShortDescription("Child 1 command")
        .withLongDescription("The first child command")
        .withExecutionFunc([](const cli::CliContext &ctx) {
            ctx.Logger.info() << "Child 1 executed" << std::flush;
        });

    auto child2Cmd = cli::commands::Command("child2");
    child2Cmd.withShortDescription("Child 2 command")
        .withLongDescription("The second child command")
        .withExecutionFunc([](const cli::CliContext &ctx) {
            ctx.Logger.info() << "Child 2 executed" << std::flush;
        });

    parentCmd.withSubCommand(std::move(child1Cmd));
    parentCmd.withSubCommand(std::move(child2Cmd));

    app.withCommand(std::move(parentCmd));
}

int main(int argc, char *argv[])
{
    auto config = cli::CliConfig();
    config.title = "Subcommand Demo";
    config.executableName = "subcommand_demo";
    config.description = "Subcommand Demo to test the CLI Library";
    config.version = "1.0.0";

    auto cliApp = cli::CliApp(std::move(config));
    initCommands(cliApp);

    RUN_CLI_APP(cliApp, argc, argv);
}