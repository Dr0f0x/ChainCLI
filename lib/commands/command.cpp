#include "command.h"

#include <format>
#include <iostream>

#include "cli_app.h"
#include "docwriting/docwriting.h"

namespace cli::commands
{
std::string_view Command::getDocStringShort() const
{
    if (docStringShort.empty())
    {
        throw docwriting::DocsNotBuildException(
            std::format("Short documentation string not built for command '{}'.", identifier));
    }
    return docStringShort;
}

std::string_view Command::getDocStringLong() const
{
    if (docStringLong.empty())
    {
        throw docwriting::DocsNotBuildException(
            std::format("Long documentation string not built for command '{}'.", identifier));
    }
    return docStringLong;
}

void Command::execute(const CliContext &context) const
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

Command &Command::withShortDescription(std::string_view desc)
{
    shortDescription = desc;
    return *this;
}

Command &Command::withLongDescription(std::string_view desc)
{
    longDescription = desc;
    return *this;
}

Command &Command::withExecutionFunc(
    std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
{
    executePtr = std::move(actionPtr);
    return *this;
}

Command &Command::withSubCommand(std::unique_ptr<Command> subCommandPtr)
{
    subCommands.try_emplace(subCommandPtr->identifier, std::move(subCommandPtr));
    return *this;
}

void Command::safeAddToArgGroup(const std::shared_ptr<ArgumentBase> &arg)
{
    if (indexForNewArgGroup >= argumentGroups.size())
    {
        argumentGroups.push_back(std::make_unique<ArgumentGroup>(false, false));
    }
    argumentGroups[indexForNewArgGroup]->addArgument(arg);
}

void Command::addArgGroup(const ArgumentGroup &argGroup)
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

Command *Command::getSubCommand(std::string_view id)
{
    auto it = subCommands.find(id);
    return (it != subCommands.end()) ? it->second.get() : nullptr;
}

const Command *Command::getSubCommand(std::string_view id) const
{
    auto it = subCommands.find(id);
    return (it != subCommands.end()) ? it->second.get() : nullptr;
}

std::ostream &operator<<(std::ostream &out, const Command &cmd)
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

std::string MalformedCommandException::buildMessage(const Command &cmd, const std::string &msg)
{
    std::ostringstream oss;
    oss << "Malformed Command: " << cmd.getIdentifier();
    if (!msg.empty())
        oss << " - " << msg;
    return oss.str();
}

} // namespace cli::commands
