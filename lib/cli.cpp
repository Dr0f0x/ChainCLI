#include "cli.h"
#include <iostream>

namespace cli {
    void print(const std::string& msg) {
        std::cout << "[cli] " << msg << std::endl;
    }
}