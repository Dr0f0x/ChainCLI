#include "command.h"
#include "docwriting/docwriting.h"
#include <iostream>
#include <format>
#include "clibase.h"

namespace cli::commands
{
    std::string_view Command::getDocStringShort() const
    {
        if (docStringShort.empty())
        {
            throw DocsNotBuildException(
                std::format("Short documentation string not built for command '{}'.", identifier));
        }
        return docStringShort;
    }

    std::string_view Command::getDocStringLong() const
    {
        if (docStringLong.empty())
        {
            throw DocsNotBuildException(
                std::format("Long documentation string not built for command '{}'.", identifier));
        }
        return docStringLong;
    }

    void Command::execute() const
    {
        if (executePtr && *executePtr)
        {
            (*executePtr)();
        }
        else
        {
            cli::CLI().Logger().error("Command {} currently has no execute function", identifier);
        }
    }

    void Command::buildDocStrings()
    {
        docStringShort = docwriting::generateShortDocString(*this);
        docStringLong = docwriting::generateLongDocString(*this);
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

    Command &Command::withArgument(std::unique_ptr<Argument> &&arg)
    {
        arguments.push_back(std::move(arg));
        return *this;
    }

    Command &Command::withExecutionFunc(std::unique_ptr<std::function<void()>> actionPtr)
    {
        executePtr = std::move(actionPtr);
        return *this;
    }

    std::ostream &operator<<(std::ostream &out, const Command &cmd)
    {
        out << "Command - " << cmd.identifier
            << " ( short Description: " << cmd.shortDescription
            << "; long Description: " << cmd.longDescription
            << "; arguments: [";

        for (size_t i = 0; i < cmd.arguments.size(); ++i)
        {
            if (cmd.arguments[i])
                out << *(cmd.arguments[i]); // assumes Argument has operator<<
            if (i + 1 < cmd.arguments.size())
                out << ", ";
        }

        out << "])";
        return out;
    }

} // namespace cli::commands
