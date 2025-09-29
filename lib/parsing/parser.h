#pragma once
#include <string>
#include <sstream>
#include <type_traits>
#include <iostream>
#include <vector>
#include <memory>
#include <any>
#include "commands/argument.h"

namespace cli::parsing
{
    void test();

    class ParseException : public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };

    class StringParser
    {
    public:
        template <typename T>
        static T parse(const std::string &input)
        {
            std::istringstream iss(input);
            T value;

            if constexpr (std::is_same_v<T, std::string>)
            {
                return input;  // For strings, just return
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

        static std::vector<std::any> parseArguments(const std::vector<std::unique_ptr<cli::commands::PositionalArgument>> &arguments,
            const std::vector<std::string>& inputs);
    };
}