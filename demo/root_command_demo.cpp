#include "chain_cli.hpp"

void rootFunc(const cli::CliContext &ctx)
{
    auto input = ctx.getPositionalArg<std::string>("input");
    ctx.Logger.info() << "Input: " << input << '\n';
    ctx.Logger.info() << "This is the root command"
                      << std::flush;
}

int main(int argc, char *argv[])
{
    auto config = cli::CliConfig();
    config.title = "Root Command Demo";
    config.executableName = "root_command_demo";
    config.description = "Root Command Demo to test the CLI Library";
    config.version = "1.0.0";

    cli::CliApp app(std::move(config));

    // Define the root command with a positional argument and an execution function
    auto& rootCommand = *(app.getMainCommand());
    rootCommand.withLongDescription("The root command of the CLI application")
        .withShortDescription("The root command")
        .withPositionalArgument(cli::commands::PositionalArgument<std::string>("input")
            .withOptionsComment("A string input for the root command")
            .withRequired(true))
        .withExecutionFunc(rootFunc);
                                  
    RUN_CLI_APP(app, argc, argv);
}