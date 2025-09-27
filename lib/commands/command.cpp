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

    Command &Command::withArgument(std::unique_ptr<Argument>& arg)
    {
        arguments.push_back(std::move(arg));
        return *this;
    }

    std::ostream &operator<<(std::ostream &out, const Command &cmd)
    {
        out << "Command - " << cmd.identifier
            << " ( short Description: " << cmd.shortDescription
            << "; long Description: " << cmd.longDescription
            << "; arguments: [";

        for (size_t i = 0; i < cmd.arguments.size(); ++i)
        {
            if (cmd.arguments[i])
                out << *(cmd.arguments[i]); // assumes Argument has operator<<
            if (i + 1 < cmd.arguments.size())
                out << ", ";
        }

        out << "])";
        return out;
    }

} // namespace cli::commands
