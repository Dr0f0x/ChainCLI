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