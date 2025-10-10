#include "chain_cli.hpp"

void inclusiveFunc(const cli::CliContext &ctx)
{
    auto input = ctx.getPositionalArg<std::string>("input");
    bool hasValue = ctx.isOptionArgPresent("--value");

    ctx.Logger.info() << "Input: " << input;

    if (hasValue)
    {
        auto value = ctx.getOptionArg<int>("--value");
        auto name = ctx.getOptionArg<std::string>("--name");
        ctx.Logger.info() << "Value: " << value;
        ctx.Logger.info() << " Name: " << name << std::flush;
    }
    else
    {
        ctx.Logger.info() << "No inclusive group arguments provided." << std::flush;
    }
}

void exclusiveFunc(const cli::CliContext &ctx)
{
    auto input = ctx.getPositionalArg<std::string>("input");
    bool hasValue = ctx.isOptionArgPresent("--value");
    bool hasName = ctx.isOptionArgPresent("--name");

    ctx.Logger.info() << "Input: " << input;

    if (hasValue)
    {
        auto value = ctx.getOptionArg<int>("--value");
        ctx.Logger.info() << "Value: " << value << std::flush;
    }
    else if (hasName)
    {
        auto name = ctx.getOptionArg<std::string>("--name");
        ctx.Logger.info() << "Name: " << name << std::flush;
    }
    else
    {
        ctx.Logger.info() << "No exclusive group arguments provided." << std::flush;
    }
}

void initCommands(cli::CliApp &app)
{
    auto inclusiveCommand = std::move(cli::commands::Command("inclusive")
        .withShortDescription("A command that uses an inclusive argument group")
        .withLongDescription("A command that has an inclusive argument group, "
                            "where if one argument is provided, the other must be provided as well.")
        .withPositionalArgument(cli::commands::PositionalArgument<std::string>("input")
                                    .withOptionsComment("A string input")
                                    .withRequired(true))
        .withInclusiveGroup(cli::commands::OptionArgument<int>("--value", "number")
                                .withShortName("-v")
                                .withOptionsComment("First argument in inclusive group")
                                .withRequired(false),
                            cli::commands::OptionArgument<std::string>("--name", "text")
                                .withShortName("-n")
                                .withOptionsComment("Second argument in inclusive group")
                                .withRequired(false))
        .withExecutionFunc(inclusiveFunc));

    auto exclusiveCommand = std::move(cli::commands::Command("exclusive")
        .withShortDescription("A command that uses an exclusive argument group")
        .withLongDescription("A command that has an exclusive argument group, "
                            "where if one argument is provided, the other must not be provided.")
        .withPositionalArgument(cli::commands::PositionalArgument<std::string>("input")
                                    .withOptionsComment("A string input")
                                    .withRequired(true))
        .withExclusiveGroup(cli::commands::OptionArgument<int>("--value", "number")
                                .withShortName("-v")
                                .withOptionsComment("First argument in exclusive group")
                                .withRequired(false),
                            cli::commands::OptionArgument<std::string>("--name", "text")
                                .withShortName("-n")
                                .withOptionsComment("Second argument in exclusive group")
                                .withRequired(false))
        .withExecutionFunc(exclusiveFunc));

    app.withCommand(std::move(inclusiveCommand));
    app.withCommand(std::move(exclusiveCommand));
}

int main(int argc, char *argv[])
{
    auto config = cli::CliConfig();
    config.title = "Argument Group Demo";
    config.executableName = "argument_group_demo";
    config.description = "Argument Group Demo to test the CLI Library";
    config.version = "1.0.0";

    auto cliApp = cli::CliApp(std::move(config));

    initCommands(cliApp);

    RUN_CLI_APP(cliApp, argc, argv);
}