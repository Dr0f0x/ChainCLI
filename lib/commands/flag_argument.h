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
        FlagArgument(std::string_view name,
                     std::string_view shortName,
                     std::string_view usage_comment = "")
            : ArgumentBase(name, usage_comment, false),
              FlaggedArgumentBase(shortName) {}

        std::string getOptionsDocString() const override;
        std::string getArgDocString() const override;
    };
}