#include "clibase.h"
#include <iostream>

int command_func(){
    std::cout << "command called" << std::endl;
    return 0;
}

int main(int argc, char* argv[]) {
    cli::print("Hello from demo!");

    cli::CliBase cli;

    auto helpCmd = std::make_unique<cli::commands::Command>("run", "Run something", command_func);
    cli.addCommand(std::move(helpCmd));

    auto cmd = cli.getCommand("run");
    if (cmd) {
        std::cout << *cmd << std::endl;
    }

    cli.run(argc, argv);

    return 0;
}