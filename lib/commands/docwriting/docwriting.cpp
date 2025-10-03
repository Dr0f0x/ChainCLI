#include <sstream>
#include <string>
#include "commands/argument.h"
#include "docwriting.h"

namespace cli::commands::docwriting
{
    std::pair<char, char> getPositionalArgumentBrackets(bool required)
    {
        if (required)
            return {'<', '>'};
        else
            return {'[', ']'};
    }

    std::pair<char, char> getOptionArgumentBrackets(bool required)
    {
        if (required)
            return {'(', ')'};
        else
            return {'[', ']'};
    }

    void addGroupArgumentDocString(std::ostringstream &builder, const cli::commands::ArgumentGroup &groupArgs)
    {
        auto [inBracket, outBracket] = getOptionArgumentBrackets(groupArgs.isRequired());
        if (groupArgs.isExclusive() || groupArgs.isInclusive())
        {
            builder << inBracket;
        }

        auto args = groupArgs.getArguments();
        for (size_t i = 0; i < args.size(); ++i)
        {
            const auto &argPtr = args[i];
            builder << argPtr->getArgDocString();

            if (i < args.size() - 1) // not the last element
            {
                if (groupArgs.isExclusive())
                    builder << " | ";
                else
                    builder << " ";
            }
        }

        if (groupArgs.isExclusive() || groupArgs.isInclusive())
        {
            builder << outBracket;
        }
    }

    std::string generateShortDocString(const Command &command)
    {
        std::ostringstream builder;
        builder << command.getIdentifier() << " ";

        for (const auto &argGroupPtr : command.getArgumentGroups())
        {
            addGroupArgumentDocString(builder, *argGroupPtr);
            builder << " ";
        }
        builder << "\n"
                << command.getShortDescription();
        return builder.str();
    }

    std::string generateLongDocString(const Command &command)
    {
        std::ostringstream builder;
        builder << command.getIdentifier() << " ";

        for (const auto &argGroupPtr : command.getArgumentGroups())
        {
            addGroupArgumentDocString(builder, *argGroupPtr);
            builder << ' ';
        }

        builder << "\n\n"
                << command.getLongDescription() << "\n\nOptions:\n";

        for (const auto &argGroupPtr : command.getArgumentGroups())
        {
            for (const auto &argPtr : argGroupPtr->getArguments())
            {
                builder << argPtr->getOptionsDocString() << "\n";
            }
        }
        return builder.str();
    }

    std::string generateOptionsDocString(const FlagArgument &argument)
    {
        std::ostringstream builder;
        builder << argument.getName() << ' ' << argument.getShortName() << '\t' << argument.getUsageComment();
        return builder.str();
    }

    std::string generateArgDocString(const FlagArgument &argument)
    {
        std::ostringstream builder;
        auto [inBracket, outBracket] = getOptionArgumentBrackets(argument.isRequired());
        builder << inBracket << argument.getName() << ',' << argument.getShortName() << outBracket;
        return builder.str();
    }

    std::string generateOptionsDocString(const OptionArgumentBase &argument)
    {
        std::ostringstream builder;
        builder << argument.getName() << ',' << argument.getShortName() << ' ';
        builder << '<' << argument.getValueName() << '>' << '\t' << argument.getUsageComment();
        return builder.str();
    }

    std::string generateArgDocString(const OptionArgumentBase &argument)
    {
        std::ostringstream builder;
        auto [inBracket, outBracket] = getOptionArgumentBrackets(argument.isRequired());
        builder << inBracket << argument.getName() << ' ' << argument.getShortName() << ' ';
        builder << '<' << argument.getValueName() << '>' << outBracket;
        return builder.str();
    }

    std::string generateOptionsDocString(const PositionalArgumentBase &argument)
    {
        std::ostringstream builder;
        auto [inBracket, outBracket] = getPositionalArgumentBrackets(argument.isRequired());

        builder << inBracket << argument.getName();
        builder << outBracket << '\t' << argument.getUsageComment();
        return builder.str();
    }

    std::string generateArgDocString(const PositionalArgumentBase &argument)
    {
        std::ostringstream builder;
        auto [inBracket, outBracket] = getPositionalArgumentBrackets(argument.isRequired());

        builder << inBracket << argument.getName();
        builder << outBracket;
        return builder.str();
    }
} // namespace cli::commands::docwriting