#pragma once
#include "cli_config.h"
#include "commands/argument.h"
#include "commands/argument_group.h"
#include "commands/command.h"
#include "commands/flag_argument.h"
#include "commands/option_argument.h"
#include "commands/positional_argument.h"

namespace cli::commands::docwriting
{

class DocsNotBuildException : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};

class DocWriter
{

public:
    explicit DocWriter(const CliConfig &config) : configuration(config) {}

    void setDocStrings(Command &command, std::string_view fullCommandPath) const;

    std::string generateLongDocString(const Command &command,
                                      std::string_view fullCommandPath) const;

    std::string generateShortDocString(const Command &command,
                                       std::string_view fullCommandPath) const;

    std::string generateOptionsDocString(const FlagArgument &argument) const;

    std::string generateArgDocString(const FlagArgument &argument) const;

    std::string generateOptionsDocString(const OptionArgumentBase &argument) const;

    std::string generateArgDocString(const OptionArgumentBase &argument) const;

    std::string generateOptionsDocString(const PositionalArgumentBase &argument) const;

    std::string generateArgDocString(const PositionalArgumentBase &argument) const;

private:
    void addGroupArgumentDocString(std::ostringstream &builder,
                                   const cli::commands::ArgumentGroup &groupArgs) const;
    const CliConfig &configuration;
};

} // namespace cli::commands::docwriting