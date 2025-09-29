#include <sstream>
#include <string>
#include <commands/argument.h>
#include "docwriting.h"

namespace cli::commands::docwriting
{
    std::pair<char, char> getArgumentBrackets(const PositionalArgument& argument)
    {
        if (argument.isRequired())
            return {'<', '>'};
        else
            return {'[', ']'};
    }

    std::string generateOptionsDocString(const PositionalArgument &argument)
    {
        std::ostringstream builder;
        auto [inBracket, outBracket] = getArgumentBrackets(argument);

        builder << inBracket << argument.getName();
        if (argument.hasShortName())
            builder << "," + std::string(argument.getShortName());
        builder << outBracket << '\t' << argument.getUsageComment();
        return builder.str();
    }

    std::string generateArgDocString(const PositionalArgument &argument)
    {
        std::ostringstream builder;
        auto [inBracket, outBracket] = getArgumentBrackets(argument);

        builder << inBracket << argument.getName();
        if (argument.hasShortName())
            builder << "," + std::string(argument.getShortName()) << outBracket;
        return builder.str();
    }

    std::string generateShortDocString(const Command &command)
    {
        std::ostringstream builder;
        builder << command.getIdentifier() << " ";
        for (const auto &argPtr : command.getArguments())
        {
            if (argPtr)
            {
                builder << argPtr->getArgDocString() << " ";
            }
        }
        builder << "\n" << command.getShortDescription();
        return builder.str();
    }

    std::string generateLongDocString(const Command &command)
    {
        std::ostringstream builder;
        builder << command.getIdentifier() << " ";
        for (const auto &argPtr : command.getArguments())
        {
            if (argPtr)
            {
                builder << argPtr->getArgDocString() << " ";
            }
        }
        builder << "\n\n" << command.getLongDescription() << "\n\nOptions:\n";
        for (const auto &argPtr : command.getArguments())
        {
            if (argPtr)
            {
                builder << argPtr->getOptionsDocString() << "\n";
            }
        }
        return builder.str();
    }
}// namespace cli::commands::docwriting