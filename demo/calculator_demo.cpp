#include <cmath>

#include "chain_cli.hpp"

void add(const cli::CliContext &ctx)
{
    double res = 0;
    for (const auto &val : ctx.getRepeatableArg<double>("summand"))
    {
        res += val;
    }
    ctx.Logger.info("Result: {}", res);
}

void multiply(const cli::CliContext &ctx)
{
    double res = 1;
    for (const auto &val : ctx.getRepeatableArg<double>("factor"))
    {
        res *= val;
    }
    ctx.Logger.info("Result: {}", res);
}

void subtract(const cli::CliContext &ctx)
{
    auto minuend = ctx.getPositionalArg<double>("minuend");
    double bound = minuend + 1; // just to have an initial value
    bool boundPresent = ctx.isOptionArgPresent("--bound");

    if (boundPresent)
    {
        bound = ctx.getOptionArg<double>("--bound");
        if (minuend < bound)
        {
            ctx.Logger.debug("Initial minuend {} is already below the bound {}", minuend, bound);
        }
    }

    for (const auto &val : ctx.getRepeatableArg<double>("subtrahend"))
    {
        minuend -= val;
        if (boundPresent && minuend < bound)
        {
            ctx.Logger.info("Subtraction crossed the bound of {}: current value is {}", bound,
                            minuend);
        }
    }
    ctx.Logger.info("Result: {}", minuend);
}

void divide(const cli::CliContext &ctx)
{
    auto dividend = ctx.getPositionalArg<double>("dividend");
    bool useRemainder = ctx.isFlagPresent("--remainder");

    if (useRemainder)
        ctx.Logger.info("Using remainders for division");

    for (const auto &val : ctx.getRepeatableArg<double>("divisor"))
    {
        if (val == 0)
        {
            ctx.Logger.error("Division by zero is not allowed!");
            return;
        }

        if (useRemainder)
        {
            dividend = std::fmod(dividend, val);
        }
        else
        {
            dividend /= val;
        }
    }

    if (useRemainder)
        ctx.Logger.info("Remainder result: {}", dividend);
    else
        ctx.Logger.info("Division result: {}", dividend);
}

void initCommands(cli::CliApp &cliApp)
{
    // verbose way of defining commands
    auto addCommand = std::move(cli::commands::Command("add")
        .withShortDescription("Add the provided numbers")
        .withLongDescription("Adds all numbers given with the <summand> parameter and prints "
                                "the result to the console.")
        .withPositionalArgument(cli::commands::createPositionalArgument<double>("summand")
                                    .withOptionsComment("All the numbers to add")
                                    .withRequired(true)
                                    .withRepeatable(true))
        .withExecutionFunc(add));
    cliApp.withCommand(std::move(addCommand));

    // more concise way using constructors
    cliApp.withCommand(
        std::move(cli::commands::Command("mult", "Multiply the provided numbers",
                                         "Multiplies all number given with the <factor> argument "
                                         "and prints the result to the console",
                                         multiply)
                      .withPositionalArgument(cli::commands::PositionalArgument<double>(
                          "factor", "All the numbers to multiply", true, true))));

    // verbose way of defining commands
    auto subCommand = std::move(cli::commands::Command("sub")
        .withShortDescription("Subtracts all the numbers after the first one from it")
        .withLongDescription("Subtracts all numbers given with the <subtrahend> "
                                "parameter from the number given with the <minuend> "
                                "parameter and prints the result to the console.")
        .withPositionalArgument(cli::commands::PositionalArgument<double>("minuend")
                                    .withOptionsComment("The number that is subtracted from")
                                    .withRequired(true))
        .withPositionalArgument(cli::commands::PositionalArgument<double>("subtrahend")
                                    .withOptionsComment("All the numbers to subtract")
                                    .withRequired(true)
                                    .withRepeatable(true))
        .withOptionArgument(
            cli::commands::OptionArgument<double>("--bound", "lowest")
                .withShortName("-b")
                .withOptionsComment("specify a lower bound to display a message if the "
                                    "subtraction process crosses it"))
        .withExecutionFunc(subtract));
    cliApp.withCommand(std::move(subCommand));

    // more concise way using constructors
    cliApp.withCommand(std::move(
        cli::commands::Command("div", "Divide the first argument by all args after it",
                               "Divides the number passe dwith the <dividend> argument by all "
                               "values passed with the <dividend> argument successfully",
                               subtract)
            .withPositionalArgument(cli::commands::PositionalArgument<double>(
                "dividend", "The number that is divided", true, false))
            .withPositionalArgument(cli::commands::PositionalArgument<double>(
                "divisor", "The numbers to divide by", true, true))
            .withFlagArgument(cli::commands::FlagArgument(
                "--remainder", "-r", "Divide using remainders instead of precise", false))
            .withExecutionFunc(divide)));
}

const int widerOptionsWidth = 23;

int main(int argc, char *argv[])
{
    auto config = cli::CliConfig();
    config.title = "Calculator Demo";
    config.executableName = "calculator_demo";
    config.description = "Calculator Demo to test the CLI Library";
    config.optionsWidth = widerOptionsWidth;
    config.version = "1.0.0";

    auto cliApp = cli::CliApp(std::move(config));

    initCommands(cliApp);

    RUN_CLI_APP(cliApp, argc, argv);
}