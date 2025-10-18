#pragma once

#include <string>
#include "chain_cli.hpp"

namespace Heady
{
struct Params
{
    std::string sourceFolder;
    std::string output;
    std::string excluded;
    std::string inlined;
    std::string define;
    std::string licenseHeader;
    bool recursiveScan;
    bool includeFileHints;
    bool useStandardIncludeGuard;

    cli::logging::AbstractLogger &logger;

    // Constructor
    explicit Params(cli::logging::AbstractLogger &log) 
        : sourceFolder(""), 
          output(""), 
          excluded(""), 
          inlined("inline_t"), 
          define(""), 
          licenseHeader(""),
          recursiveScan(false), 
          includeFileHints(true),
          useStandardIncludeGuard(false),
          logger(log) 
    {
    }
};
} //namespace Heady