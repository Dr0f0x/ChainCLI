#pragma once
#include <memory>

#include "cli_config.h"
#include "commands/argument.h"
#include "commands/argument_group.h"
#include "commands/command.h"
#include "commands/docwriting/docformatter.h"
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

    void setOptionFormatter(std::unique_ptr<AbstractArgDocFormatter<OptionArgumentBase>> formatter);

    void setPositionalFormatter(
        std::unique_ptr<AbstractArgDocFormatter<PositionalArgumentBase>> formatter);

    void setFlagFormatter(std::unique_ptr<AbstractArgDocFormatter<FlagArgument>> formatter);

    void setCommandFormatter(std::unique_ptr<AbstractCommandFormatter> formatter);

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
    const CliConfig &configuration;

    std::unique_ptr<AbstractCommandFormatter> commandFormatterPtr = std::make_unique<DefaultCommandFormatter>();
    std::unique_ptr<AbstractArgDocFormatter<FlagArgument>> flagFormatterPtr  = std::make_unique<DefaultFlagFormatter>();
    std::unique_ptr<AbstractArgDocFormatter<OptionArgumentBase>> optionFormatterPtr  = std::make_unique<DefaultOptionFormatter>();
    std::unique_ptr<AbstractArgDocFormatter<PositionalArgumentBase>> positionalFormatterPtr = std::make_unique<DefaultPositionalFormatter>();
};

} // namespace cli::commands::docwriting