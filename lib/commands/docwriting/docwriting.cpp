#include "docwriting.h"
#include "commands/argument.h"
#include <format>
#include <iomanip>
#include <sstream>
#include <string>

#define inline_t

namespace cli::commands::docwriting
{

void DocWriter::setOptionFormatter(
    std::unique_ptr<AbstractArgDocFormatter<OptionArgumentBase>> formatter)
{
    optionFormatterPtr = std::move(formatter);
}

void DocWriter::setPositionalFormatter(
    std::unique_ptr<AbstractArgDocFormatter<PositionalArgumentBase>> formatter)
{
    positionalFormatterPtr = std::move(formatter);
}

void DocWriter::setFlagFormatter(std::unique_ptr<AbstractArgDocFormatter<FlagArgument>> formatter)
{
    flagFormatterPtr = std::move(formatter);
}

void DocWriter::setCommandFormatter(std::unique_ptr<AbstractCommandFormatter> formatter)
{
    commandFormatterPtr = std::move(formatter);
}

void DocWriter::setDocStrings(Command &command, std::string_view fullCommandPath) const
{
    command.docStringLong = generateLongDocString(command, fullCommandPath);
    command.docStringShort = generateShortDocString(command, fullCommandPath);
}

std::string DocWriter::generateShortDocString(const Command &command,
                                              std::string_view fullCommandPath) const
{
    return commandFormatterPtr->generateShortDocString(command, fullCommandPath, *this, configuration);
}


std::string DocWriter::generateLongDocString(const Command &command,
                                             std::string_view fullCommandPath) const
{
    return commandFormatterPtr->generateLongDocString(command, fullCommandPath, *this, configuration);
}

std::string DocWriter::generateOptionsDocString(const FlagArgument &argument) const
{
    return flagFormatterPtr->generateOptionsDocString(argument, configuration);
}

std::string DocWriter::generateArgDocString(const FlagArgument &argument) const
{
    return flagFormatterPtr->generateArgDocString(argument, configuration);
}

std::string DocWriter::generateOptionsDocString(const OptionArgumentBase &argument) const
{
    return optionFormatterPtr->generateOptionsDocString(argument, configuration);
}

std::string DocWriter::generateArgDocString(const OptionArgumentBase &argument) const
{
    return optionFormatterPtr->generateArgDocString(argument, configuration);
}

std::string DocWriter::generateOptionsDocString(const PositionalArgumentBase &argument) const
{
    return positionalFormatterPtr->generateOptionsDocString(argument, configuration);
}

std::string DocWriter::generateArgDocString(const PositionalArgumentBase &argument) const
{
    return positionalFormatterPtr->generateArgDocString(argument, configuration);
}

} // namespace cli::commands::docwriting