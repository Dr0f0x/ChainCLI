#include "command.h"
#include <iostream>

namespace cli::commands {

    void test()
    {
        std::cout << "Output from command module" << std::endl;
    }


    std::ostream &operator<<(std::ostream &out, const Command &cmd)
    {
        out << "Command: " << cmd.getIdentifier()
            << "\nDescription: " << cmd.getDescription();
        return out;
    }

    void Command::execute() const
    {
        if (executeFunc) executeFunc();
        else std::cout << "no execute function passed" << std::endl;
    }

} // namespace cli::commands
