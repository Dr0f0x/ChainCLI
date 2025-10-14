/*
 * Copyright 2025 Dominik Czekai
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <chrono>
#include <string>

#include "loglevel.h"

namespace cli::logging
{

/// @brief Data structure representing a log record.
struct LogRecord
{
    /// @brief The log level of the record.
    const LogLevel level;
    /// @brief The log message.
    const std::string message;
    /// @brief The timestamp of the log record.
    const std::chrono::system_clock::time_point timestamp{std::chrono::system_clock::now()};

    /// @brief Construct a new LogRecord.
    /// @param lvl The log level of the record.
    /// @param msg The log message.
    LogRecord(LogLevel lvl, std::string msg) : level(lvl), message(std::move(msg)) {}
};

} // namespace cli::logging