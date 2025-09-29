#include "parser.h"
#include <iostream>

void cli::parsing::test()
{
    std::cout << "Output from parsing module" << std::endl;
}

std::vector<std::any> cli::parsing::StringParser::parseArguments(const std::vector<std::unique_ptr<cli::commands::PositionalArgument>> &arguments, const std::vector<std::string> &inputs)
{
    if (inputs.size() < arguments.size()) {
        throw ParseException("Not enough arguments provided");
    }

    std::vector<std::any> parsed;
    parsed.reserve(arguments.size());

    for (size_t i = 0; i < arguments.size(); ++i) {
        const auto& arg = *arguments[i];
        const auto& input = inputs[i];

        //TODO there must be a better way to do this (options are turn argument into a template or attach a parse callback)
        if (arg.getType() == typeid(int)) {
            parsed.push_back(parse<int>(input));
        } else if (arg.getType() == typeid(double)) {
            parsed.push_back(parse<double>(input));
        } else if (arg.getType() == typeid(std::string)) {
            parsed.push_back(parse<std::string>(input));
        } else {
            throw ParseException("Unsupported argument type for: " + std::string(arg.getName()));
        }
    }

    return parsed;
}
