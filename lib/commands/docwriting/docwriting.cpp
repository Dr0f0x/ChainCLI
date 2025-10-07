#include "docwriting.h"
#include "commands/argument.h"
#include <format>
#include <iomanip>
#include <sstream>
#include <string>

#define inline_t

namespace cli::commands::docwriting
{

inline_t std::pair<std::string, std::string> getPositionalArgumentBrackets(bool required)
{
    if (required)
        return {"<", ">"};
    else
        return {"[<", ">]"};
}

inline_t std::pair<char, char> getOptionArgumentBrackets(bool required)
{
    if (required)
        return {'(', ')'};
    else
        return {'[', ']'};
}

void DocWriter::addGroupArgumentDocString(std::ostringstream &builder,
                                          const cli::commands::ArgumentGroup &groupArgs) const
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
        builder << argPtr->getArgDocString(*this);

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

std::string DocWriter::generateShortDocString(const Command &command,
                                              std::string_view fullCommandPath) const
{
    std::ostringstream builder;
    builder << fullCommandPath << " ";

    for (const auto &argGroupPtr : command.getArgumentGroups())
    {
        addGroupArgumentDocString(builder, *argGroupPtr);
        builder << " ";
    }
    builder << "\n" << command.getShortDescription();
    return builder.str();
}

void DocWriter::setDocStrings(Command &command, std::string_view fullCommandPath) const
{
    command.docStringLong = generateLongDocString(command, fullCommandPath);
    command.docStringShort = generateShortDocString(command, fullCommandPath);
}

std::string DocWriter::generateLongDocString(const Command &command,
                                             std::string_view fullCommandPath) const
{
    std::ostringstream builder;
    builder << fullCommandPath << " ";

    for (const auto &argGroupPtr : command.getArgumentGroups())
    {
        addGroupArgumentDocString(builder, *argGroupPtr);
        builder << ' ';
    }

    builder << "\n\n" << command.getLongDescription() << "\n\nOptions:\n";

    for (const auto &argGroupPtr : command.getArgumentGroups())
    {
        for (const auto &argPtr : argGroupPtr->getArguments())
        {
            builder << argPtr->getOptionsDocString(*this) << "\n";
        }
    }
    return builder.str();
}

std::string DocWriter::generateOptionsDocString(const FlagArgument &argument) const
{
    std::ostringstream builder;
    builder << argument.getName() << ' ' << argument.getShortName();
    return std::format("{:<{}}{:>{}}", builder.str(), 20, argument.getUsageComment(),
                       argument.getUsageComment().size());
}

std::string DocWriter::generateArgDocString(const FlagArgument &argument) const
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getOptionArgumentBrackets(argument.isRequired());
    builder << inBracket << argument.getName() << ',' << argument.getShortName() << outBracket;
    return builder.str();
}

std::string DocWriter::generateOptionsDocString(const OptionArgumentBase &argument) const
{
    std::ostringstream builder;
    builder << argument.getName() << ',' << argument.getShortName() << ' ' << '<'
            << argument.getValueName() << '>';
    if (argument.isRepeatable())
        builder << "...";
    return std::format("{:<{}}{:>{}}", builder.str(), 20, argument.getUsageComment(),
                       argument.getUsageComment().size());
}

std::string DocWriter::generateArgDocString(const OptionArgumentBase &argument) const
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getOptionArgumentBrackets(argument.isRequired());
    builder << inBracket << argument.getName() << ' ' << argument.getShortName() << ' ';
    builder << '<' << argument.getValueName() << '>' << outBracket;

    if (argument.isRepeatable())
        builder << "...";
    return builder.str();
}

std::string DocWriter::generateOptionsDocString(const PositionalArgumentBase &argument) const
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getPositionalArgumentBrackets(argument.isRequired());

    builder << inBracket << argument.getName() << outBracket;
    if (argument.isRepeatable())
        builder << "...";
    return std::format("{:<{}}{:>{}}", builder.str(), 20, argument.getUsageComment(),
                       argument.getUsageComment().size());
}

std::string DocWriter::generateArgDocString(const PositionalArgumentBase &argument) const
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getPositionalArgumentBrackets(argument.isRequired());

    builder << inBracket << argument.getName();
    builder << outBracket;

    if (argument.isRepeatable())
        builder << "...";
    return builder.str();
}
} // namespace cli::commands::docwriting