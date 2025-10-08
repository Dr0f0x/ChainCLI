// Copyright 2025 Dominik Czekai
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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