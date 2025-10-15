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
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <any>

namespace cli
{
    class MissingArgumentException : public std::runtime_error
{
public:
    MissingArgumentException(const std::string &name,
                             const std::unordered_map<std::string, std::any> &args)
        : std::runtime_error(makeMessage(name, args))
    {
    }

private:
    static std::string makeMessage(const std::string &name,
                                   const std::unordered_map<std::string, std::any> &args);
};

class InvalidArgumentTypeException : public std::runtime_error
{
public:
    InvalidArgumentTypeException(const std::string &name, const std::type_info &requested,
                                 const std::type_info &actual)
        : std::runtime_error(makeMessage(name, requested, actual))
    {
    }

private:
    static std::string makeMessage(const std::string &name, const std::type_info &requested,
                                   const std::type_info &actual);
};
} // namespace cli
