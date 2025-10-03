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
                              std::string_view usageComment = "",
                              bool isRequired = false)
            : ArgumentBase(name, usageComment, ArgumentKind::Flag, isRequired),
              FlaggedArgumentBase(shortName) {}

        std::string getOptionsDocString() const override;
        std::string getArgDocString() const override;
    };
}