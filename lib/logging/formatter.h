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

#include "logrecord.h"

namespace cli::logging
{

/// @brief Interface for log record formatters.
class AbstractFormatter
{
public:
    virtual ~AbstractFormatter() = default;

    /// @brief Formats a log record into a string.
    /// @param record The log record to format.
    /// @return The formatted log record as a string.
    virtual std::string format(const LogRecord &record) const = 0;
};


/// @brief Basic implementation of a log record formatter that includes timestamp, log level, and message.
class BasicFormatter : public AbstractFormatter
{
public:
    std::string format(const LogRecord &record) const override;

private:
    const std::chrono::time_zone *timezone = std::chrono::current_zone();
};


/// @brief Formatter that displays only the message of the log record.
class MessageOnlyFormatter : public AbstractFormatter
{
public:
    std::string format(const LogRecord &record) const override;
};

} // namespace cli::logging