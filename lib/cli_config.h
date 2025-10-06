#pragma once
#include <string>

namespace cli
{
    struct CliConfig
    {
        // General CLI metadata
        std::string title;
        std::string executableName; // e.g. argv[0]
        std::string description;    // human-readable description
        std::string version;        // e.g. "1.2.3"

        // Runtime flags and options
        char repeatableDelimiter{','};

        // Behavior toggles
        // ...
    };

}