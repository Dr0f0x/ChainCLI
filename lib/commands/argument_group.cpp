#include <algorithm>
#include "argument_group.h"

namespace cli::commands
{
    bool ArgumentGroup::isRequired() const
    {
        if (exclusive)
        {
            return std::ranges::all_of(arguments,
                                       [](auto const &arg)
                                       { return arg->isRequired(); });
        }
        if (inclusive)
        {
            return std::ranges::any_of(arguments,
                                       [](auto const &arg)
                                       { return arg->isRequired(); });
        }
        return false;
    }
}