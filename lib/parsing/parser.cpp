#include "parser.h"
#include <iostream>
#include "commands/command.h"
#include "parser_utils.h"
#include "cli_base.h"

void cli::parsing::test()
{
    std::cout << "Output from parsing module" << std::endl;
}

std::vector<std::any> cli::parsing::StringParser::parsePositionalArguments(
    const std::vector<std::unique_ptr<cli::commands::ArgumentBase>> &arguments,
    const std::vector<std::string> &inputs,
    ContextBuilder &ContextBuilder)
{
    if (inputs.size() < arguments.size())
    {
        throw ParseException("Not enough arguments provided");
    }

    std::vector<std::any> parsed;
    parsed.reserve(arguments.size());

    for (size_t i = 0; i < arguments.size(); ++i)
    {
        const auto &arg = *arguments[i];
        const auto &input = inputs[i];

        auto val = arg.parseToValue(input);
        ContextBuilder.addPositionalArgument(arg.getName(), val);

        parsed.push_back(val);
    }

    return parsed;
}
