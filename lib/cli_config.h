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
#include <string>

namespace cli
{
struct CliConfig
{
    // General CLI metadata
    std::string title;
    std::string executableName; // e.g. argv[0]
    std::string description;    // human-readable description
    std::string version;        // e.g. "1.2.3"

    // Runtime flags and options
    char repeatableDelimiter{','};
    int optionsWidth{20}; // width that is used to right aling the options text for arguments

    // Behavior toggles
    // ...
};

} // namespace cli