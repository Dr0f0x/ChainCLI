#include "argument.h"
#include <string_view>
#include "docwriting/docwriting.h"

namespace cli::commands
{
    std::ostream &operator<<(std::ostream &out, const Argument &arg)
    {
        out << arg.name 
            << " (" << arg.usageComment << ")";
        return out;
    }

    std::string Argument::getOptionsDocString() const
    {
        return docwriting::generateOptionsDocString(*this);
    }

    std::string Argument::getArgDocString() const
    {
        return docwriting::generateArgDocString(*this);
    }

    Argument &Argument::withShortName(std::string_view short_name)
    {
        shortName = short_name;
        return *this;
    }

    Argument &Argument::withUsageComment(std::string_view usage_comment)
    {
        usageComment = usage_comment;
        return *this;
    }

    Argument &Argument::withRequired(bool req)
    {
        required = req;
        return *this;
    }

} // namespace cli::commands