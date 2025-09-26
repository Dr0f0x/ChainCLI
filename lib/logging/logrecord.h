#pragma once
#include "loglevel.h"
#include <string>
#include <chrono>

namespace cli::logging
{

    struct LogRecord
    {
        LogLevel level;
        std::string message;
        std::chrono::system_clock::time_point timestamp{std::chrono::system_clock::now()};

        LogRecord(LogLevel lvl, std::string msg)
            : level(lvl), message(std::move(msg)) {}
    };

} // namespace cli::logging