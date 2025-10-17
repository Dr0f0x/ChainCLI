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

#include "command.h"

#include <format>
#include <iostream>

#include "cli_app.h"
#include "docwriting/docs_exception.h"
#include "docwriting/docwriting.h"

#define inline_t

namespace cli::commands
{

inline_t std::string_view Command::getDocStringShort() const
{
    if (docStringShort.empty())
    {
        throw docwriting::DocsNotBuildException(
            std::format("Short documentation string not built for command '{}'.", identifier), *this);
    }
    return docStringShort;
}

inline_t std::string_view Command::getDocStringLong() const
{
    if (docStringLong.empty())
    {
        throw docwriting::DocsNotBuildException(
            std::format("Long documentation string not built for command '{}'.", identifier), *this);
    }
    return docStringLong;
}

inline_t void Command::execute(const CliContext &context) const
{
    if (executePtr && *executePtr)
    {
        (*executePtr)(context);
    }
    else
    {
        // cli::CLI().Logger().error("Command {} currently has no execute function",
        // identifier);
    }
}

inline_t Command &Command::withShortDescription(std::string_view desc)
{
    shortDescription = desc;
    return *this;
}

inline_t Command &Command::withLongDescription(std::string_view desc)
{
    longDescription = desc;
    return *this;
}

inline_t Command &Command::withFlagArgument(std::shared_ptr<FlagArgument> arg)
{
    safeAddToArgGroup(arg);
    flagArguments.push_back(arg);
    return *this;
}

inline_t Command &Command::withFlagArgument(FlagArgument &&arg)
{
    return withFlagArgument(std::make_shared<FlagArgument>(std::move(arg)));
}

inline_t Command &Command::withFlagArgument(FlagArgument &arg)
{
    return withFlagArgument(std::make_shared<FlagArgument>(arg));
}

inline_t Command &Command::withExecutionFunc(
    std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
{
    executePtr = std::move(actionPtr);
    return *this;
}

inline_t Command &Command::withExecutionFunc(std::function<void(const CliContext &)> &&action)
{
    return withExecutionFunc(
        std::make_unique<std::function<void(const CliContext &)>>(std::move(action)));
}

inline_t Command &Command::withSubCommand(std::unique_ptr<Command> subCommandPtr)
{
    subCommands.try_emplace(subCommandPtr->identifier, std::move(subCommandPtr));
    return *this;
}

inline_t Command &Command::withSubCommand(Command &&subCommand)
{
    return withSubCommand(std::make_unique<Command>(std::move(subCommand)));
}

inline_t void Command::safeAddToArgGroup(const std::shared_ptr<ArgumentBase> &arg)
{
    if (indexForNewArgGroup >= argumentGroups.size())
    {
        argumentGroups.push_back(std::make_unique<ArgumentGroup>(false, false));
    }
    argumentGroups[indexForNewArgGroup]->addArgument(arg);
}

inline_t void Command::addArgGroup(const ArgumentGroup &argGroup)
{
    for (auto &arg : argGroup.getArguments())
    {
        switch (arg->getArgType())
        {
        case ArgumentKind::Flag:
            flagArguments.push_back(std::static_pointer_cast<FlagArgument>(arg));
            break;

        case ArgumentKind::Positional:
            positionalArguments.push_back(std::static_pointer_cast<PositionalArgumentBase>(arg));
            break;

        case ArgumentKind::Option:
            optionArguments.push_back(std::static_pointer_cast<OptionArgumentBase>(arg));
            break;
        }
    }
}

inline_t Command *Command::getSubCommand(std::string_view id)
{
    auto it = subCommands.find(id);
    return (it != subCommands.end()) ? it->second.get() : nullptr;
}

inline_t const Command *Command::getSubCommand(std::string_view id) const
{
    auto it = subCommands.find(id);
    return (it != subCommands.end()) ? it->second.get() : nullptr;
}

inline_t std::ostream &operator<<(std::ostream &out, const Command &cmd)
{
    out << "Command - " << cmd.identifier << " ( short Description: " << cmd.shortDescription
        << "; long Description: " << cmd.longDescription << "; arguments: [";

    for (size_t i = 0; i < cmd.positionalArguments.size(); ++i)
    {
        // if (cmd.arguments[i])
        // out << *(cmd.arguments[i]); // assumes Argument has operator<<
        if (i + 1 < cmd.positionalArguments.size())
            out << ", ";
    }

    out << "])";
    return out;
}

inline_t std::string MalformedCommandException::buildMessage(const Command &cmd, const std::string &msg)
{
    std::ostringstream oss;
    oss << "Malformed Command: " << cmd.getIdentifier();
    if (!msg.empty())
        oss << " - " << msg;
    return oss.str();
}

} // namespace cli::commands
