#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "logging/formatter.h"
#include "logging/handler.h"
#include "logging/logger.h"

// Mock formatter
class MockFormatter : public cli::logging::AbstractFormatter
{
public:
    MOCK_METHOD(std::string, format, (const cli::logging::LogRecord &record), (const, override));
};

// Mock handler
class MockHandler : public cli::logging::BaseHandler
{
public:
    explicit MockHandler(std::unique_ptr<cli::logging::AbstractFormatter> f)
        : BaseHandler(std::cout, std::cerr, std::move(f))
    {
    }

    MOCK_METHOD(void, emit, (const cli::logging::LogRecord &record), (const, override));
};