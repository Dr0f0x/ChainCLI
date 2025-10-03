#pragma once
#include <stdexcept>

namespace cli::parsing
{
    class ParseException : public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };

    class GroupParseException : public std::runtime_error
    {
    public:
        using runtime_error::runtime_error;
    };

}