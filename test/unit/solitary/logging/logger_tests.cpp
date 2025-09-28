#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mocks.h"
#include "logging/logger.h"
#include "logging/handler.h"
#include "logging/formatter.h"

using namespace cli::logging;
using ::testing::_;
using ::testing::AllOf;
using ::testing::Eq;
using ::testing::Exactly;
using ::testing::Field;
using ::testing::Return;

class LoggerTestSolitary : public ::testing::Test {
public:
    std::unique_ptr<MockFormatter> mockFormatterPtr;
    MockFormatter* mockFormatterRawPtr = nullptr;
    std::unique_ptr<MockHandler> mockHandlerPtr;
    MockHandler* mockHandlerRawPtr = nullptr;

    void SetUp() override {
        mockFormatterPtr = std::make_unique<MockFormatter>();
        mockFormatterRawPtr = mockFormatterPtr.get();
        mockHandlerPtr = std::make_unique<MockHandler>(std::move(mockFormatterPtr));
        mockHandlerRawPtr = mockHandlerPtr.get();
    }
};

TEST_F(LoggerTestSolitary, LoggerCallsHandlerWhichCallsFormatter)
{
    Logger logger(LogLevel::TRACE);
    logger.addHandler(std::move(mockHandlerPtr));

    ON_CALL(*mockHandlerRawPtr, emit(testing::_))
        .WillByDefault([this](const LogRecord &r)
                       { this->mockFormatterRawPtr->format(r); });

    EXPECT_CALL(*mockHandlerRawPtr, emit(AllOf(
                                 Field(&LogRecord::level, Eq(LogLevel::INFO)),
                                 Field(&LogRecord::message, Eq("original message")))))
        .Times(1);

    logger.info("original message");
}

TEST_F(LoggerTestSolitary, DoesNotCallHandlerBelowMinLevel)
{
    Logger logger(LogLevel::ERROR);
    logger.addHandler(std::move(mockHandlerPtr));

    // Formatter and handler should NOT be called
    EXPECT_CALL(*mockFormatterRawPtr, format(_)).Times(0);
    EXPECT_CALL(*mockHandlerRawPtr, emit(_)).Times(0);

    logger.info("ignored message");
}

TEST_F(LoggerTestSolitary, LoggerFormatsArgumentsBeforeEmit)
{
    Logger logger(LogLevel::TRACE);
    logger.addHandler(std::move(mockHandlerPtr));

    EXPECT_CALL(*mockHandlerRawPtr, emit(AllOf(
                                 Field(&LogRecord::level, Eq(LogLevel::INFO)),
                                 Field(&LogRecord::message, Eq("Value=42")))))
        .Times(1);

    int val = 42;
    logger.info("Value={}", val);
}

TEST_F(LoggerTestSolitary, ConvenienceMethodsUsesCorrectLevel)
{
    Logger logger(LogLevel::TRACE);
    logger.addHandler(std::move(mockHandlerPtr));

    ON_CALL(*mockHandlerRawPtr, emit(testing::_))
        .WillByDefault([this](const LogRecord &r)
                       { this->mockFormatterRawPtr->format(r); });

    EXPECT_CALL(*mockFormatterRawPtr, format(_))
        .Times(Exactly(1));

    EXPECT_CALL(*mockHandlerRawPtr, emit(testing::Truly([](const LogRecord& r) {
        return r.level == LogLevel::WARNING && r.message == "warning message";
    }))).Times(1);

    logger.warning("warning message");
}

TEST_F(LoggerTestSolitary, CorrectLevelsPassedToHandler)
{
    Logger logger(LogLevel::TRACE);
    logger.addHandler(std::move(mockHandlerPtr));

    // Expect correct level for trace
    EXPECT_CALL(*mockHandlerRawPtr, emit(testing::Truly([](const LogRecord& r) {
        return r.level == LogLevel::TRACE && r.message == "trace msg";
    })));
    logger.trace("trace msg");
}


TEST_F(LoggerTestSolitary, RemoveAllHandlersPreventsEmits)
{
    Logger logger(LogLevel::TRACE);
    logger.addHandler(std::move(mockHandlerPtr));

    logger.removeAllHandlers();

    // Nothing should be called
    EXPECT_NO_THROW(logger.info("any message"));
}

struct LoggerMethodCase {
    LogLevel level;
    std::function<void(Logger&, const std::string&)> method;
    const char* msg;
};

class LoggerConvenienceParamTest : public ::testing::TestWithParam<LoggerMethodCase> {};

TEST_P(LoggerConvenienceParamTest, ConvenienceMethodCallsHandlerWithCorrectLevel) {
    auto mockFormatterPtr = std::make_unique<MockFormatter>();
    MockFormatter const* mockFormatterRawPtr = mockFormatterPtr.get();

    auto mockHandlerPtr = std::make_unique<MockHandler>(std::move(mockFormatterPtr));
    MockHandler const* mockHandlerRawPtr = mockHandlerPtr.get();

    Logger logger(LogLevel::TRACE);
    logger.addHandler(std::move(mockHandlerPtr));

    // Forward call from handler to formatter
    ON_CALL(*mockHandlerRawPtr, emit(testing::_))
        .WillByDefault([mockFormatterRawPtr](const LogRecord& r) {
            mockFormatterRawPtr->format(r);
        });

    EXPECT_CALL(*mockFormatterRawPtr, format(_)).Times(1);

    EXPECT_CALL(*mockHandlerRawPtr, emit(testing::Truly([](const LogRecord& r) {
        return r.level == GetParam().level && r.message == GetParam().msg;
    }))).Times(1);

    // Run the logger method
    GetParam().method(logger, GetParam().msg);
}

INSTANTIATE_TEST_SUITE_P(
    DataTestsSolitary,
    LoggerConvenienceParamTest,
    ::testing::Values(
        LoggerMethodCase{LogLevel::TRACE,   [](auto& l, auto& m){ l.trace(m); },   "trace message"},
        LoggerMethodCase{LogLevel::DEBUG,   [](auto& l, auto& m){ l.debug(m); },   "debug message"},
        LoggerMethodCase{LogLevel::INFO,    [](auto& l, auto& m){ l.info(m); },    "info message"},
        LoggerMethodCase{LogLevel::WARNING, [](auto& l, auto& m){ l.warning(m); }, "warning message"},
        LoggerMethodCase{LogLevel::ERROR,   [](auto& l, auto& m){ l.error(m); },   "error message"}
    )
);

struct LoggerLevelCase {
    LogLevel level;
    std::string msg;
};

class LoggerLevelParamTest : public ::testing::TestWithParam<LoggerLevelCase> {};

TEST_P(LoggerLevelParamTest, CorrectLevelIsPassedToHandler)
{
    auto mockFormatterPtr = std::make_unique<MockFormatter>();
    auto mockHandlerPtr = std::make_unique<MockHandler>(std::move(mockFormatterPtr));
    MockHandler const *mockHandlerRawPtr = mockHandlerPtr.get();

    Logger logger(LogLevel::TRACE);
    logger.addHandler(std::move(mockHandlerPtr));

    EXPECT_CALL(*mockHandlerRawPtr, emit(testing::Truly([](const LogRecord& r) {
        return r.level == GetParam().level && r.message == GetParam().msg;
    }))).Times(1);

    // Call the convenience method via pointer-to-member
    logger.log(GetParam().level, GetParam().msg);
}

INSTANTIATE_TEST_SUITE_P(
    DataTestsSolitary,
    LoggerLevelParamTest,
    ::testing::Values(
        LoggerLevelCase{LogLevel::TRACE,    "trace message"},
        LoggerLevelCase{LogLevel::DEBUG,    "debug message"},
        LoggerLevelCase{LogLevel::INFO,     "info message"},
        LoggerLevelCase{LogLevel::WARNING,  "warning message"},
        LoggerLevelCase{LogLevel::ERROR,    "error message"}
    )
);