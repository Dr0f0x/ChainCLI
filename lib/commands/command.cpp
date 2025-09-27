#include "command.h"
#include <iostream>

namespace cli::commands
{

    void Command::execute() const
    {
        if (executePtr && *executePtr)
        {
            (*executePtr)();
        }
        else
        {
            std::cout << "no execute function passed" << std::endl;
        }
    }

    std::ostream &operator<<(std::ostream &out, const Command &cmd)
    {
        out << "Command - " << cmd.getIdentifier()
            << " ( short Description: " << cmd.getShortDescription() << "; "
            << "long Description" << cmd.getLongDescription() << " )";
        return out;
    }

} // namespace cli::commands
