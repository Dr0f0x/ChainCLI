#pragma once
#include "logrecord.h"
#include <string>

namespace cli::logging
{

    class IFormatter
    {
    public:
        virtual ~IFormatter() = default;
        virtual std::string format(const LogRecord &record) const = 0;
    };

    // A simple formatter with timestamp + level + message
    class BasicFormatter : public IFormatter
    {
    public:
        std::string format(const LogRecord &record) const override;
    };

    // A formatter that displays only the message
    class MessageOnlyFormatter : public IFormatter
    {
    public:
        std::string format(const LogRecord &record) const override;
    };

} // namespace cli::logging