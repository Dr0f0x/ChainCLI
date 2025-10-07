#include <gtest/gtest.h>

#include <sstream>

#include "logging/formatter.h"
#include "logging/handler.h"
#include "logging/logger.h"

using namespace cli::logging;

class LoggerTestSociable : public ::testing::Test
{
public:
    std::ostringstream out;
    std::ostringstream err;
};

TEST_F(LoggerTestSociable, LoggerCallsHandlerWhichCallsFormatter)
{
    Logger logger(LogLevel::TRACE);
    logger.addHandler(std::make_unique<Handler>(out, err, std::make_unique<BasicFormatter>()));

    logger.info("original message");

    std::string result = out.str();
    EXPECT_NE(result.find("original message"), std::string::npos);
    EXPECT_NE(result.find("["), std::string::npos);
}

TEST_F(LoggerTestSociable, DoesNotCallHandlerBelowMinLevel)
{
    Logger logger(LogLevel::ERROR);
    logger.addHandler(
        std::make_unique<Handler>(out, err, std::make_unique<MessageOnlyFormatter>()));

    logger.info("ignored message");

    EXPECT_TRUE(out.str().empty());
    EXPECT_TRUE(err.str().empty());
}

TEST_F(LoggerTestSociable, LoggerFormatsArgumentsBeforeEmit)
{
    Logger logger(LogLevel::TRACE);
    logger.addHandler(
        std::make_unique<Handler>(out, err, std::make_unique<MessageOnlyFormatter>()));

    int val = 42;
    logger.info("Value={}", val);

    EXPECT_NE(out.str().find("Value=42"), std::string::npos);
}

TEST_F(LoggerTestSociable, ConvenienceMethodsUsesCorrectLevel)
{
    Logger logger(LogLevel::TRACE);
    logger.addHandler(std::make_unique<Handler>(out, err, std::make_unique<BasicFormatter>()));

    logger.warning("warning message");

    std::string result = err.str() + out.str();
    EXPECT_NE(result.find("WARNING"), std::string::npos);
    EXPECT_NE(result.find("warning message"), std::string::npos);
}

TEST_F(LoggerTestSociable, CorrectLevelsPassedToHandler)
{
    Logger logger(LogLevel::TRACE);
    logger.addHandler(
        std::make_unique<Handler>(out, err, std::make_unique<BasicFormatter>(), LogLevel::TRACE));

    logger.trace("trace msg");
    std::string result = out.str() + err.str();
    EXPECT_NE(result.find("TRACE"), std::string::npos);
}

TEST_F(LoggerTestSociable, RemoveAllHandlersPreventsEmits)
{
    Logger logger(LogLevel::TRACE);
    logger.addHandler(std::make_unique<Handler>(out, err, std::make_unique<MessageOnlyFormatter>(),
                                                LogLevel::TRACE));
    logger.removeAllHandlers();

    EXPECT_NO_THROW(logger.info("any message"));
    EXPECT_TRUE(out.str().empty());
    EXPECT_TRUE(err.str().empty());
}

struct LoggerMethodCase
{
    LogLevel level;
    std::function<void(Logger &, const std::string &)> method;
    const char *msg;
};

class LoggerConvenienceParamTest : public ::testing::TestWithParam<LoggerMethodCase>
{
};

TEST_P(LoggerConvenienceParamTest, ConvenienceMethodCallsHandlerWithCorrectLevel)
{
    std::ostringstream out;
    std::ostringstream err;
    Logger logger(LogLevel::TRACE);
    logger.addHandler(
        std::make_unique<Handler>(out, err, std::make_unique<BasicFormatter>(), LogLevel::TRACE));

    GetParam().method(logger, GetParam().msg);

    std::string result = out.str() + err.str();
    EXPECT_NE(result.find(GetParam().msg), std::string::npos);
}

INSTANTIATE_TEST_SUITE_P(
    DataTestsSociable, LoggerConvenienceParamTest,
    ::testing::Values(
        LoggerMethodCase{LogLevel::TRACE, [](auto &l, const std::string &m) { l.trace(m); },
                         "trace message"},
        LoggerMethodCase{LogLevel::DEBUG, [](auto &l, const std::string &m) { l.debug(m); },
                         "debug message"},
        LoggerMethodCase{LogLevel::INFO, [](auto &l, const std::string &m) { l.info(m); },
                         "info message"},
        LoggerMethodCase{LogLevel::WARNING, [](auto &l, const std::string &m) { l.warning(m); },
                         "warning message"},
        LoggerMethodCase{LogLevel::ERROR, [](auto &l, const std::string &m) { l.error(m); },
                         "error message"}));

struct LoggerLevelCase
{
    LogLevel level;
    std::string msg;
};

class LoggerLevelParamTest : public ::testing::TestWithParam<LoggerLevelCase>
{
};

TEST_P(LoggerLevelParamTest, CorrectLevelIsPassedToHandler)
{
    std::ostringstream out;
    std::ostringstream err;
    Logger logger(LogLevel::TRACE);
    logger.addHandler(
        std::make_unique<Handler>(out, err, std::make_unique<BasicFormatter>(), LogLevel::TRACE));

    logger.log(GetParam().level, GetParam().msg);

    std::string result = out.str() + err.str();
    EXPECT_NE(result.find(GetParam().msg), std::string::npos);
}

INSTANTIATE_TEST_SUITE_P(DataTestsSociable, LoggerLevelParamTest,
                         ::testing::Values(LoggerLevelCase{LogLevel::TRACE, "trace message"},
                                           LoggerLevelCase{LogLevel::DEBUG, "debug message"},
                                           LoggerLevelCase{LogLevel::INFO, "info message"},
                                           LoggerLevelCase{LogLevel::WARNING, "warning message"},
                                           LoggerLevelCase{LogLevel::ERROR, "error message"}));
