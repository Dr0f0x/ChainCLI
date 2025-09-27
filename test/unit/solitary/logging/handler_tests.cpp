#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>
#include "mocks.h"
#include "logging/handler.h"
#include "logging/formatter.h"
#include "logging/loglevel.h"
#include "logging/logstyle.h"

using namespace cli::logging;
using ::testing::Ref;
using ::testing::Return;

// Test subclass to inject streams
class TestHandler : public Handler
{
public:
    TestHandler(std::unique_ptr<IFormatter> f, std::ostream &outStream, std::ostream &errStream, LogLevel minLevel = LogLevel::DEBUG)
        : Handler(outStream, errStream, std::move(f), minLevel) {}
};

class HandlerTestSolitary : public ::testing::Test {
public:
    std::ostringstream out;
    std::ostringstream err;
    std::unique_ptr<MockFormatter> mockFormatterPtr;
    MockFormatter* mockFormatterRawPtr = nullptr;

    void SetUp() override {
        // Create formatter, keep raw pointer for mocks
        mockFormatterPtr = std::make_unique<MockFormatter>();
        mockFormatterRawPtr = mockFormatterPtr.get();
    }
};

TEST_F(HandlerTestSolitary, EmitCallsFormatter)
{
    Handler handler(out, err, std::move(mockFormatterPtr));

    LogRecord debugRecord(LogLevel::DEBUG, "debug-msg");
    LogRecord errorRecord(LogLevel::ERROR, "error-msg");

    EXPECT_CALL(*mockFormatterRawPtr, format(Ref(debugRecord)))
        .WillOnce(Return("MOCK_DEBUG"));
    EXPECT_CALL(*mockFormatterRawPtr, format(Ref(errorRecord)))
        .WillOnce(Return("MOCK_ERROR"));

    handler.emit(debugRecord);
    handler.emit(errorRecord);
}

TEST_F(HandlerTestSolitary, EmitWritesToCorrectStream)
{
    Handler handler(out, err, std::move(mockFormatterPtr));

    LogRecord debugRecord(LogLevel::DEBUG, "debug-msg");
    LogRecord errorRecord(LogLevel::ERROR, "error-msg");

    ON_CALL(*mockFormatterRawPtr, format(testing::_))
        .WillByDefault([](const LogRecord &r)
                       { return "MOCK_" + r.message; });

    handler.emit(debugRecord);
    handler.emit(errorRecord);

    EXPECT_NE(out.str().find("MOCK_debug-msg"), std::string::npos);
    EXPECT_NE(err.str().find("MOCK_error-msg"), std::string::npos);
}

TEST_F(HandlerTestSolitary, EmitIgnoresBelowMinLevel)
{
    Handler handler(out, err, std::move(mockFormatterPtr), LogLevel::ERROR);

    LogRecord warningRecord(LogLevel::WARNING, "ignored-msg");

    // Formatter should never be called
    EXPECT_CALL(*mockFormatterRawPtr, format(testing::_)).Times(0);

    handler.emit(warningRecord); // no crash

    EXPECT_EQ(out.str(), "");
    EXPECT_EQ(err.str(), "");
}

TEST_F(HandlerTestSolitary, ConsoleHandlerEmitsCorrectly)
{
    // Redirect std::cout and std::cerr
    std::streambuf *oldCout = std::cout.rdbuf(out.rdbuf());
    std::streambuf *oldCerr = std::cerr.rdbuf(err.rdbuf());

    ConsoleHandler handler(std::move(mockFormatterPtr));

    LogRecord infoRecord(LogLevel::INFO, "info-msg");
    LogRecord errorRecord(LogLevel::ERROR, "error-msg");

    EXPECT_CALL(*mockFormatterRawPtr, format(Ref(infoRecord))).WillOnce(Return("MOCK_INFO"));
    EXPECT_CALL(*mockFormatterRawPtr, format(Ref(errorRecord))).WillOnce(Return("MOCK_ERROR"));

    handler.emit(infoRecord);
    handler.emit(errorRecord);

    // Restore original buffers
    std::cout.rdbuf(oldCout);
    std::cerr.rdbuf(oldCerr);

    EXPECT_NE(out.str().find("MOCK_INFO"), std::string::npos);
    EXPECT_NE(err.str().find("MOCK_ERROR"), std::string::npos);
}

TEST_F(HandlerTestSolitary, StylingAppliedToFormattedMessage)
{
    auto styles = defaultStyles();
    Handler handler(out, err, std::move(mockFormatterPtr));

    handler.setStylingEnabled(true);
    handler.setStyleMap(std::make_shared<LogStyleMap>(styles));

    LogRecord infoRecord(LogLevel::INFO, "info-msg");

    EXPECT_CALL(*mockFormatterRawPtr, format(Ref(infoRecord)))
        .WillOnce(Return("FORMATTED"));

    handler.emit(infoRecord);

    std::string expected = styles[LogLevel::INFO] + "FORMATTED" + "\o{33}[0m";
    EXPECT_EQ(out.str(), expected);
}

TEST_F(HandlerTestSolitary, StylingDisabledDoesNotModifyMessage)
{
    Handler handler(out, err, std::move(mockFormatterPtr));

    handler.setStylingEnabled(false);
    handler.setStyleMap(std::make_shared<LogStyleMap>(defaultStyles()));

    LogRecord infoRecord(LogLevel::INFO, "info-msg");

    EXPECT_CALL(*mockFormatterRawPtr, format(Ref(infoRecord)))
        .WillOnce(Return("FORMATTED"));

    handler.emit(infoRecord);

    std::string result = out.str();
    EXPECT_EQ(result, "FORMATTED"); // no styling applied
}

TEST_F(HandlerTestSolitary, StylingAppliedOnlyIfLevelMatches)
{
    Handler handler(out, err, std::move(mockFormatterPtr));

    handler.setStylingEnabled(true);

    auto styleMap = std::make_shared<LogStyleMap>();
    (*styleMap)[LogLevel::ERROR] = "\o{33}[31m"; // red
    handler.setStyleMap(styleMap);

    LogRecord infoRecord(LogLevel::INFO, "info-msg");
    LogRecord errorRecord(LogLevel::ERROR, "error-msg");

    EXPECT_CALL(*mockFormatterRawPtr, format(Ref(infoRecord)))
        .WillOnce(Return("INFOFORMATTED"));
    EXPECT_CALL(*mockFormatterRawPtr, format(Ref(errorRecord)))
        .WillOnce(Return("ERRORFORMATTED"));

    handler.emit(infoRecord);  // INFO not in styleMap
    handler.emit(errorRecord); // ERROR has style

    EXPECT_EQ(out.str(), "INFOFORMATTED");                                             // INFO unstyled
    EXPECT_NE(err.str().find("\o{33}[31mERRORFORMATTED\o{33}[0m"), std::string::npos); // ERROR styled
}