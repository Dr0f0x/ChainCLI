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
#include <iostream>
#include <sstream>
#include <string>
#include <format>

#include "parse_exception.h"

namespace cli::parsing
{
/// @brief Helper struct providing static methods for parsing strings into various types.
struct ParseHelper
{
    /// @brief Parses a string input into a value of type T.
    /// @tparam T The type to parse the input into.
    /// @param input The input string to parse.
    /// @return The parsed value of type T.
    template <typename T> static T parse(const std::string &input)
    {
        std::istringstream iss(input);
        T value;

        if constexpr (std::is_same_v<T, std::string>)
        {
            return input; // For strings, just return
        }
        else
        {
            // Use operator>> for all other types
            iss >> value;
            if (iss.fail() || !iss.eof())
            {
                throw TypeParseException(std::format("Failed to parse value of type {} from input: {}", typeid(T).name(), input), input, typeid(T));
            }
        }

        return value;
    }

    /// @brief Parses a string input into a value of type T.
    /// @tparam T The type to parse the input into.
    /// @param input The input string to parse.
    /// @param value The variable to store the parsed value.
    template <typename T> static void parse(const std::string &input, T &value)
    {
        // Call the return-by-value version and assign
        value = parse<T>(input);
    }
};
} // namespace cli::parsing