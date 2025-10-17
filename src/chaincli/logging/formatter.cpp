// Copyright 2025 Dominik Czekai
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "formatter.h"

#include <chrono>
#include <iomanip>
#include <sstream>

#define inline_t

namespace cli::logging
{

inline_t std::string BasicFormatter::format(const LogRecord &record) const
{
    auto const time = timezone->to_local(record.timestamp);

    std::ostringstream oss;
    oss << "[" << std::format("{:%Y-%m-%d %X}", time) << "] " << toString(record.level) << ": "
        << record.message << "\n";
    return oss.str();
}

inline_t std::string MessageOnlyFormatter::format(const LogRecord &record) const
{
    return record.message + "\n";
}

} // namespace cli::logging