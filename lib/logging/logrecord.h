#pragma once
#include "loglevel.h"
#include <string>
#include <chrono>

namespace cli::logging
{

    struct LogRecord
    {
        const LogLevel level;
        const std::string message;
        const std::chrono::system_clock::time_point timestamp{std::chrono::system_clock::now()};

        // Constructor initializes all fields; fields are immutable
        LogRecord(LogLevel lvl, std::string msg)
            : level(lvl),
              message(std::move(msg))
        {}
    };

} // namespace cli::logging