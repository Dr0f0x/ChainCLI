#pragma once
#include <string>
#include <vector>
#include <memory>
#include "cli_context.h"
#include "commands/command.h"
#include "cli_config.h"

namespace cli::parsing
{
    class StringParser
    {
    public:
        explicit StringParser(const CliConfig &config)
            : configuration(config) {}

        void parseArguments(
            const cli::commands::Command &command,
            const std::vector<std::string> &inputs,
            ContextBuilder &contextBuilder);

    private:
        std::any parseRepeatableList(
            const cli::commands::TypedArgumentBase &arg,
            const std::string &input) const;

        void parseRepeatable(
            const cli::commands::OptionArgumentBase &arg,
            const std::string &input,
            ContextBuilder &ContextBuilder) const;

        void parseRepeatable(
            const cli::commands::PositionalArgumentBase &arg,
            const std::string &input,
            ContextBuilder &ContextBuilder) const;

        void checkGroups(
            const cli::commands::Command &command,
            const ContextBuilder &contextBuilder) const;

        bool tryOptionArg(const std::vector<std::shared_ptr<cli::commands::OptionArgumentBase>> &optionArguments,
                          const std::vector<std::string> &inputs,
                          const std::string &currentParsing,
                          int index,
                          ContextBuilder &contextBuilder) const;

        bool tryFlagArg(const std::vector<std::shared_ptr<cli::commands::FlagArgument>> &flagArguments,
                        const std::string &currentParsing,
                        ContextBuilder &contextBuilder) const;

        const CliConfig &configuration;
    };
}