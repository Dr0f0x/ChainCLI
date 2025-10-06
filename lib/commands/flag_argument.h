#pragma once
#include <ostream>
#include <string>
#include <any>
#include <typeindex>
#include "argument.h"
#include "parsing/parser_utils.h"

namespace cli::commands
{
    class FlagArgument : public ArgumentBase, public FlaggedArgumentBase
    {
    public:
        explicit FlagArgument(std::string_view name,
                              std::string_view shortName = "",
                              std::string_view optionsComment = "",
                              bool isRequired = false)
            : ArgumentBase(name, optionsComment, ArgumentKind::Flag, false, isRequired),
              FlaggedArgumentBase(shortName) {}

        std::string getOptionsDocString() const override;
        std::string getArgDocString() const override;

        FlagArgument &withOptionsComment(std::string_view usage_comment)
        {
            optionsComment = usage_comment;
            return *this;
        }

        FlagArgument &withRequired(bool req)
        {
            required = req;
            return *this;
        }

        FlagArgument &withShortName(std:: string_view shortName)
        {
            this->shortName = shortName;
            return *this;
        }
    };
}