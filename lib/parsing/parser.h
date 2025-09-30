#pragma once
#include <string>
#include <vector>
#include <memory>
#include <any>
#include "commands/argument.h"
#include "cli_context.h"

namespace cli::parsing
{
    void test();

    class StringParser
    {
    public:

        static std::vector<std::any> parsePositionalArguments(const std::vector<std::unique_ptr<cli::commands::ArgumentBase>> &arguments,
            const std::vector<std::string>& inputs, ContextBuilder& ContextBuilder);
    };
}