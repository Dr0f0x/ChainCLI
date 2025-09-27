#include "argument.h"
#include <string_view>

namespace cli::commands
{
    std::ostream &operator<<(std::ostream &out, const Argument &arg)
    {
        out << arg.name 
            << " (" << arg.usageComment << ")";
        return out;
    }

    std::string_view Argument::getOptionsDocString() const
    {
        return std::string_view();
    }

    std::string_view Argument::getArgDocString() const
    {
        return std::string_view();
    }

} // namespace cli::commands