#pragma once
#include <string_view>

namespace cli::logging
{

    enum class LogLevel
    {
        TRACE = 0,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
    };

    inline std::string_view toString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        }
        return "UNKNOWN";
    }

} // namespace cli::logging