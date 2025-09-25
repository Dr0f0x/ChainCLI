#include "clibase.h"
#include "logging\logger.h"
#include "commands\command.h"
#include "parsing\parser.h"
#include <iostream>

namespace cli {
    void print(const std::string& msg) {
        std::cout << "[cli] " << msg << std::endl;
        logging::test();
        commands::test();
        parsing::test();
    }
}