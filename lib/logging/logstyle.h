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
#include <map>
#include <string>

#include "loglevel.h"

#if defined(_MSC_VER)
#define ESC "\x1B"
#else
#define ESC "\033"
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