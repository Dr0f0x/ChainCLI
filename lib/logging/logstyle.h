#pragma once
#include <map>
#include "loglevel.h"
#include <string>

namespace cli::logging
{

    // contains ANSI escape codes for styling log messages
    using LogStyleMap = std::map<LogLevel, std::string>;

    // Default console styles
    inline LogStyleMap defaultStyles()
    {

        using enum cli::logging::LogLevel;
        return {
            {TRACE,     "\o{33}[90m"},  // gray
            {VERBOSE,   "\o{33}[90m"},  // gray
            // Debug uses plain grey text (no color) 
            {DETAIL,    "\o{33}[36m"},  // cyan
            {INFO,      "\o{33}[32m"},  // green
            {WARNING,   "\o{33}[33m"},  // yellow
            {ERROR,     "\o{33}[31m"},  // red
        };
    }
} // namespace cli::logging