#pragma once
#include <string_view>

namespace cli::logging
{

    enum class LogLevel
    {
        TRACE = 0,
        DEBUG,
        DETAIL,
        INFO,
        WARNING,
        ERROR,
    };

    inline std::string_view toString(LogLevel level)
    {
        switch (level)
        {
        using enum cli::logging::LogLevel;
        case TRACE:
            return "TRACE";
        case DEBUG:
            return "DEBUG";
        case DETAIL:
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