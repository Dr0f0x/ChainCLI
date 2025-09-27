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

} // namespace cli::commands