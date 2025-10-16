#include "chain_cli.hpp"

struct CustomType
{
    friend std::istream &operator>>(std::istream &is, CustomType &ct)
    {
        // Expected format: "a,b" (e.g., "42,3.14")
        std::string input;
        is >> input;

        // Find the comma separator
        size_t commaPos = input.find(',');
        if (commaPos == std::string::npos)
        {
            is.setstate(std::ios::failbit);
            return is;
        }

        try
        {
            // Parse the integer part (before comma)
            std::string aStr = input.substr(0, commaPos);
            ct.a = std::stoi(aStr);

            // Parse the double part (after comma)
            std::string bStr = input.substr(commaPos + 1);
            ct.b = std::stod(bStr);
        }
        catch (const std::invalid_argument &)
        {
            is.setstate(std::ios::failbit);
        }
        catch (const std::out_of_range &)
        {
            is.setstate(std::ios::failbit);
        }

        return is;
    }

    int a;
    double b;
};

void customTypeFunc(const cli::CliContext &ctx)
{
    // Get the custom type from the command line argument
    auto ct = ctx.getPositionalArg<CustomType>("customArg");

    // Display the parsed custom type
    ctx.Logger().info() << "Received custom type" << "Integer part (a):" << ct.a;
    ctx.Logger().info() << "Double part (b):" << ct.b << std::flush;
}

int main(int argc, char *argv[])
{
    auto config = cli::CliConfig();
    config.title = "Custom Type Demo";
    config.executableName = "custom_type_demo";
    config.description = "Custom Type Demo to test the CLI Library";
    config.version = "1.0.0";

    auto cliApp = cli::CliApp(std::move(config));

    cliApp.withCommand(std::move(cli::commands::Command("custom")
        .withLongDescription("A command that takes a custom type as argument. "
                            "A command that takes a custom type as an argument and "
                            "parses it using the provided >> operator.")
        .withShortDescription("command with a custom type argument")
        .withPositionalArgument(
            cli::commands::PositionalArgument<CustomType>("customArg")
                .withOptionsComment(
                    "A custom type argument in format 'int,double' (e.g., '42,3.14')")
                .withRepeatable(false)
                .withRequired(true))
        .withExecutionFunc(customTypeFunc)));

    RUN_CLI_APP(cliApp, argc, argv);
}