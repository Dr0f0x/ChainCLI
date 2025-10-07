#pragma once
#include <string_view>

namespace cli::logging
{

enum class LogLevel
{
    TRACE = 0, // most detailed, prints internal logs of the library as well
    VERBOSE,   // very detailed information for inspection/development
    DEBUG,     // general debug information
    INFO,      // general information
    SUCCESS,   // report a success
    WARNING,   // something unwanted happened, but the program can handle it
    ERROR,     // a serious error occurred/ something failed
};

inline std::string_view toString(LogLevel level)
{
    switch (level)
    {
        using enum cli::logging::LogLevel;
    case TRACE:
        return "TRACE";
    case VERBOSE:
        return "VERBOSE";
    case DEBUG:
        return "DEBUG";
    case SUCCESS:
        return "DETAIL";
    case INFO:
        return "INFO";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    }
    return "UNKNOWN";
}

} // namespace cli::logging