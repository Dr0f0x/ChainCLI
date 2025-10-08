#pragma once
#include <map>
#include <string>

#include "loglevel.h"

#if defined(_MSC_VER)
  #define ESC "\x1B"
#else
  #define ESC "\o{33}"
#endif

namespace cli::logging
{

// contains ANSI escape codes for styling log messages
using LogStyleMap = std::map<LogLevel, std::string>;

// Default console styles
inline LogStyleMap defaultStyles()
{
    using enum cli::logging::LogLevel;
    return {
        {TRACE, ESC "[90m"},   // gray
        {VERBOSE, ESC "[90m"}, // gray
        {DEBUG, ESC "[36m"},   // cyan
        // Info uses plain grey text (no color)
        {SUCCESS, ESC "[32m"}, // green
        {WARNING, ESC "[33m"}, // yellow
        {ERROR, ESC "[31m"},   // red
    };
}
} // namespace cli::logging