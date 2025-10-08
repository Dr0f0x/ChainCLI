#pragma once
#include <format>
#include <sstream>

#include "cli_config.h"
#include "commands/command.h"
#include "commands/flag_argument.h"
#include "commands/option_argument.h"
#include "commands/positional_argument.h"

namespace cli::commands::docwriting
{

template <typename T> class AbstractArgDocFormatter
{
public:
    virtual ~AbstractArgDocFormatter() = default;
    virtual std::string generateArgDocString(const T &argument,
                                             const cli::CliConfig &configuration) = 0;
    virtual std::string generateOptionsDocString(const T &argument,
                                                 const cli::CliConfig &configuration) = 0;
};

class DefaultFlagFormatter : public AbstractArgDocFormatter<FlagArgument>
{
public:
    std::string generateArgDocString(const FlagArgument &argument,
                                     const cli::CliConfig &configuration) override;
    std::string generateOptionsDocString(const FlagArgument &argument,
                                         const cli::CliConfig &configuration) override;
};

class DefaultOptionFormatter : public AbstractArgDocFormatter<OptionArgumentBase>
{
public:
    std::string generateArgDocString(const OptionArgumentBase &argument,
                                     const cli::CliConfig &configuration) override;
    std::string generateOptionsDocString(const OptionArgumentBase &argument,
                                         const cli::CliConfig &configuration) override;
};

class DefaultPositionalFormatter : public AbstractArgDocFormatter<PositionalArgumentBase>
{
public:
    std::string generateArgDocString(const PositionalArgumentBase &argument,
                                     const cli::CliConfig &configuration) override;
    std::string generateOptionsDocString(const PositionalArgumentBase &argument,
                                         const cli::CliConfig &configuration) override;
};

class AbstractCommandFormatter
{
public:
    virtual ~AbstractCommandFormatter() = default;
    virtual std::string generateLongDocString(const Command &command,
                                              std::string_view fullCommandPath,
                                              const DocWriter &writer,
                                              const cli::CliConfig &configuration) = 0;

    virtual std::string generateShortDocString(const Command &command,
                                               std::string_view fullCommandPath,
                                               const DocWriter &writer,
                                               const cli::CliConfig &configuration) = 0;
};

class DefaultCommandFormatter : public AbstractCommandFormatter
{
public:
    std::string generateLongDocString(const Command &command, std::string_view fullCommandPath,
                                      const DocWriter &writer,
                                      const cli::CliConfig &configuration) override;

    std::string generateShortDocString(const Command &command, std::string_view fullCommandPath,
                                       const DocWriter &writer,
                                       const cli::CliConfig &configuration) override;
};

} // namespace cli::commands::docwriting