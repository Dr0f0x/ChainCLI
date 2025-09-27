#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "logging/logger.h"
#include "logging/handler.h"
#include "logging/formatter.h"

// Mock formatter
class MockFormatter : public cli::logging::IFormatter
{
public:
    MOCK_METHOD(std::string, format, (const cli::logging::LogRecord &record), (const, override));
};

// Mock handler
class MockHandler : public cli::logging::Handler
{
public:
    explicit MockHandler(std::unique_ptr<cli::logging::IFormatter> f)
        : Handler(std::cout, std::cerr, std::move(f)) {}

    MOCK_METHOD(void, emit, (const cli::logging::LogRecord &record), (const, override));
};