#include "chain_cli.hpp"

void genericProcessFunction(const cli::CliContext &ctx)
{
    auto filename = ctx.getPositionalArg<std::string>("filename");
    bool verbose = ctx.isFlagPresent("--verbose");
    
    ctx.Logger.info() << "filename: " << filename;
    ctx.Logger.info() << "verbose: " << (verbose ? "true" : "false") << std::flush;
}

void genericCopyFunction(const cli::CliContext &ctx)
{
    auto source = ctx.getPositionalArg<std::string>("source");
    auto destination = ctx.getRepeatableOptionArg<std::string>("--output");

    ctx.Logger.info() << "source: " << source;
    for (const auto &dest : destination)
    {
        ctx.Logger.info() << " destination: " << dest;
    }
    ctx.Logger.info() << std::flush;
}

void initCommands(cli::CliApp &app)
{
    // Command 1: positional arg + flag
    auto processCmd = cli::commands::Command("process");
    processCmd.withShortDescription("Process a file")
        .withLongDescription("Process a file with optional verbose output")
        .withPositionalArgument(
            cli::commands::PositionalArgument<std::string>("filename")
                .withOptionsComment("The file to process")
                .withRequired(true))
        .withFlagArgument(
            cli::commands::FlagArgument("--verbose", "-v")
                .withOptionsComment("Enable verbose output"))
        .withExecutionFunc(genericProcessFunction);

    // Command 2: positional arg + option arg
    auto copyCmd = cli::commands::Command("copy");
    copyCmd.withShortDescription("Copy a file")
        .withLongDescription("Copy a file to a specified destination")
        .withPositionalArgument(
            cli::commands::PositionalArgument<std::string>("source")
                .withOptionsComment("The source file to copy")
                .withRequired(true))
        .withOptionArgument(
            cli::commands::OptionArgument<std::string>("--output", "destination")
                .withShortName("-o")
                .withOptionsComment("The destination path")
                .withRequired(true)
                .withRepeatable(true))
        .withExecutionFunc(genericCopyFunction);

    app.withCommand(std::move(processCmd));
    app.withCommand(std::move(copyCmd));
}

class CustomAppDocFormatter : public cli::commands::docwriting::AbstractCliAppDocFormatter
{
public:
    CustomAppDocFormatter() = default;

    std::string generateAppDocString(const cli::CliConfig &configuration,
    const std::vector<const cli::commands::Command *> &commands) override
{
    std::ostringstream builder;
    builder << configuration.description << " - from own formatter\n\n";

    for (const auto &cmd : commands)
    {
        if (cmd->hasExecutionFunction())
            builder << cmd->getDocStringShort() << "\n\n";
    }

    builder << "Use <command> --help|-h to get more information about a specific command";
    return builder.str();
}

std::string generateCommandDocString(const cli::commands::Command &command,
                                                                const cli::CliConfig &configuration) override
{
    std::ostringstream builder;
    builder << "USAGE\n" << configuration.executableName << " " << command.getDocStringLong() << "\n\n";
    builder << command.getDocStringLong();
    return builder.str();
}

std::string generateAppVersionString(const cli::CliConfig &configuration) override
{
    return std::format("{} version from own formatter: {}", configuration.executableName, configuration.version);
}
};

class CustomOptionFormatter : public cli::commands::docwriting::DefaultOptionFormatter
{
public:
    std::string generateArgDocString(const cli::commands::OptionArgumentBase &argument,
                                     const cli::CliConfig &configuration) override
    {
        std::ostringstream builder;
        builder << "[Option: " << argument.getName();
        if (argument.isRequired())
            builder << ", Required";
        if (argument.isRepeatable())
            builder << ", Repeatable";
        builder << "]";
        return builder.str();
    }

    std::string generateOptionsDocString(const cli::commands::OptionArgumentBase &argument,
                                         const cli::CliConfig &configuration) override
    {
        std::ostringstream builder;
        builder << DefaultOptionFormatter::generateOptionsDocString(argument, configuration);

        if (argument.isRequired())
            builder << " (Required)";
        if (argument.isRepeatable())
            builder << " (Repeatable)";
        return builder.str();
    }
};

int main(int argc, char *argv[])
{
    auto config = cli::CliConfig();
    config.title = "Doc Formatter Demo";
    config.executableName = "custom_docformatter_demo";
    config.description = "Demo showing custom doc formatting";
    config.version = "1.0.0";

    auto cliApp = cli::CliApp(std::move(config));
    
    initCommands(cliApp);

    cliApp.getDocWriter().setAppFormatter(std::make_unique<CustomAppDocFormatter>());
    cliApp.getDocWriter().setOptionFormatter(std::make_unique<CustomOptionFormatter>());

    RUN_CLI_APP(cliApp, argc, argv);
}