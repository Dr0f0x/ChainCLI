#include "argument.h"
#include <string_view>
#include "docwriting/docwriting.h"

namespace cli::commands
{
    std::ostream &operator<<(std::ostream &out, const PositionalArgument &arg)
    {
        out << arg.name 
            << " (" << arg.usageComment << ")";
        return out;
    }

    std::string PositionalArgument::getOptionsDocString() const
    {
        return docwriting::generateOptionsDocString(*this);
    }

    std::string PositionalArgument::getArgDocString() const
    {
        return docwriting::generateArgDocString(*this);
    }

    PositionalArgument &PositionalArgument::withShortName(std::string_view short_name)
    {
        shortName = short_name;
        return *this;
    }

    PositionalArgument &PositionalArgument::withUsageComment(std::string_view usage_comment)
    {
        usageComment = usage_comment;
        return *this;
    }

    PositionalArgument &PositionalArgument::withRequired(bool req)
    {
        required = req;
        return *this;
    }

} // namespace cli::commands