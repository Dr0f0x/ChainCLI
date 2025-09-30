#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include "parse_exception.h"

namespace cli::parsing
{
    struct ParseHelper
    {
        template <typename T>
        static T parse(const std::string &input)
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
                    throw ParseException("Failed to parse value from input: " + input);
                }
            }

            return value;
        }

        template <typename T>
        static void parse(const std::string &input, T &value)
        {
            // Call the return-by-value version and assign
            value = parse<T>(input);
        }
    };
}