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

#include "context_exception.h"
#include <sstream>

#define inline_t

namespace cli
{
    inline_t std::string MissingArgumentException::makeMessage(
    const std::string &name, const std::unordered_map<std::string, std::any> &args)
{
    std::ostringstream oss;
    oss << "Missing argument: \"" << name << "\" was not passed in this context.\n";
    oss << "Available arguments: ";
    if (args.empty())
    {
        oss << "<none>";
    }
    else
    {
        bool first = true;
        for (auto &[k, _] : args)
        {
            if (!first)
                oss << ", ";
            oss << k;
            first = false;
        }
    }
    return oss.str();
}

inline_t std::string InvalidArgumentTypeException::makeMessage(const std::string &name,
                                                      const std::type_info &requested,
                                                      const std::type_info &actual)
{
    std::ostringstream oss;
    oss << "Invalid type for argument: \"" << name << "\"\n"
        << "Requested type: " << requested.name() << "\n"
        << "Actual type: " << actual.name();
    return oss.str();
}
} //namespace cli