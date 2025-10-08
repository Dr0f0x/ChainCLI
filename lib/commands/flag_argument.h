#pragma once
#include <any>
#include <ostream>
#include <string>
#include <typeindex>

#include "argument.h"
#include "parsing/parser_utils.h"

namespace cli::commands
{
class FlagArgument : public ArgumentBase, public FlaggedArgumentBase
{
public:
    explicit FlagArgument(std::string_view name, std::string_view shortName = "",
                          std::string_view optionsComment = "", bool isRequired = false)
        : ArgumentBase(name, optionsComment, ArgumentKind::Flag, false, isRequired),
          FlaggedArgumentBase(shortName)
    {
    }

    [[nodiscard]] std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const override;
    [[nodiscard]] std::string getArgDocString(const docwriting::DocWriter &writer) const override;

    FlagArgument &withOptionsComment(std::string_view comment)
    {
        optionsComment = comment;
        return *this;
    }

    FlagArgument &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    FlagArgument &withShortName(std::string_view name)
    {
        this->shortName = name;
        return *this;
    }
};
} // namespace cli::commands