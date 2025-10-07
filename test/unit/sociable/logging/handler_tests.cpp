#include <gtest/gtest.h>

#include <sstream>

#include "logging/formatter.h"
#include "logging/handler.h"
#include "logging/loglevel.h"
#include "logging/logstyle.h"

using namespace cli::logging;

class HandlerTestSociable : public ::testing::Test
{
public:
    std::ostringstream out;
    std::ostringstream err;
};

// Test Handler with BasicFormatter writes to correct streams
TEST_F(HandlerTestSociable, WithBasicFormatterEmitsCorrectly)
{
    auto formatter = std::make_unique<BasicFormatter>();
    Handler handler(out, err, std::move(formatter));

    LogRecord infoRecord(LogLevel::INFO, "info-msg");
    LogRecord errorRecord(LogLevel::ERROR, "error-msg");

    handler.emit(infoRecord);
    handler.emit(errorRecord);

    EXPECT_NE(out.str().find("INFO"), std::string::npos);
    EXPECT_NE(out.str().find("info-msg"), std::string::npos);
    EXPECT_NE(err.str().find("ERROR"), std::string::npos);
    EXPECT_NE(err.str().find("error-msg"), std::string::npos);
}

// Test Handler with MessageOnlyFormatter
TEST_F(HandlerTestSociable, WithMessageOnlyFormatterEmitsCorrectly)
{
    auto formatter = std::make_unique<MessageOnlyFormatter>();
    Handler handler(out, err, std::move(formatter));

    LogRecord infoRecord(LogLevel::INFO, "info-msg");
    LogRecord errorRecord(LogLevel::ERROR, "error-msg");

    handler.emit(infoRecord);
    handler.emit(errorRecord);

    EXPECT_NE(out.str().find("info-msg"), std::string::npos);
    EXPECT_NE(err.str().find("error-msg"), std::string::npos);

    // MessageOnlyFormatter does not include level or timestamp
    EXPECT_EQ(out.str().find("INFO"), std::string::npos);
    EXPECT_EQ(err.str().find("ERROR"), std::string::npos);
}

// Test styling applied with BasicFormatter
TEST_F(HandlerTestSociable, StylingAppliedWithBasicFormatter)
{
    auto formatter = std::make_unique<BasicFormatter>();
    Handler handler(out, err, std::move(formatter));

    auto styles = std::make_shared<LogStyleMap>(defaultStyles());
    handler.setStylingEnabled(true);
    handler.setStyleMap(styles);

    LogRecord infoRecord(LogLevel::INFO, "info-msg");
    LogRecord errorRecord(LogLevel::ERROR, "error-msg");

    handler.emit(infoRecord);
    handler.emit(errorRecord);

    EXPECT_NE(out.str().find((*styles)[LogLevel::INFO]), std::string::npos);
    EXPECT_NE(err.str().find((*styles)[LogLevel::ERROR]), std::string::npos);
}

// Test Handler ignores messages below min level
TEST_F(HandlerTestSociable, IgnoresMessagesBelowMinLevel)
{
    auto formatter = std::make_unique<BasicFormatter>();
    Handler handler(out, err, std::move(formatter), LogLevel::ERROR);

    LogRecord warningRecord(LogLevel::WARNING, "ignored-msg");

    handler.emit(warningRecord);

    EXPECT_TRUE(out.str().empty());
    EXPECT_TRUE(err.str().empty());
}