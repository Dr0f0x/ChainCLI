#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "logging/formatter.h"
#include "logging/logrecord.h"
#include "logging/loglevel.h"

using namespace cli::logging;

TEST(FormatterTestSolitary, BasicFormatterIncludesTimestampLevelAndMessage)
{
    BasicFormatter formatter;
    LogRecord record(LogLevel::INFO, "Hello World");

    std::string result = formatter.format(record);

    // Check for log level string
    EXPECT_NE(result.find("INFO"), std::string::npos);

    // Check for message
    EXPECT_NE(result.find("Hello World"), std::string::npos);

     // Helper lambdas
    auto containsDate = [](const std::string &s) {
        for (size_t i = 0; i + 9 < s.size(); ++i) { // [YYYY-MM-DD] = 10 chars
            if (std::isdigit(s[i+1]) && std::isdigit(s[i+2]) &&
                std::isdigit(s[i+3]) && std::isdigit(s[i+4]) &&
                s[i+5] == '-' &&
                std::isdigit(s[i+6]) && std::isdigit(s[i+7]) &&
                s[i+8] == '-' &&
                std::isdigit(s[i+9])
            ) return true;
        }
        return false;
    };

    auto containsTime = [](const std::string &s) {
        for (size_t i = 0; i + 7 < s.size(); ++i) { // HH:MM:SS = 8 chars
            if (std::isdigit(s[i]) && std::isdigit(s[i+1]) &&
                s[i+2] == ':' &&
                std::isdigit(s[i+3]) && std::isdigit(s[i+4]) &&
                s[i+5] == ':' &&
                std::isdigit(s[i+6]) && std::isdigit(s[i+7])
            ) return true;
        }
        return false;
    };

    EXPECT_TRUE(containsDate(result));
    EXPECT_TRUE(containsTime(result));
}

TEST(FormatterTestSolitary, BasicFormatterOutputsMessageWithNewline)
{
    BasicFormatter formatter;
    LogRecord record(LogLevel::ERROR, "Failure");

    std::string result = formatter.format(record);

    EXPECT_EQ(result.back(), '\n');
}
TEST(FormatterTestSolitary, MessageOnlyFormatterOutputsMessageWithNewline)
{
    MessageOnlyFormatter formatter;
    LogRecord record(LogLevel::ERROR, "Failure");

    std::string result = formatter.format(record);

    EXPECT_EQ(result.back(), '\n');
}

TEST(FormatterTestSolitary, MessageOnlyFormatterPrintsOnlyMessage) {
    MessageOnlyFormatter formatter;
    LogRecord record(LogLevel::ERROR, "Critical failure");

    std::string result = formatter.format(record);

    // It should exactly equal the message + newline
    EXPECT_EQ(result, "Critical failure\n");

    // Should not contain log level or brackets (no timestamp)
    EXPECT_EQ(result.find("ERROR"), std::string::npos);
    EXPECT_EQ(result.find("["), std::string::npos);
}
