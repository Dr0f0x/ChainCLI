#include <gtest/gtest.h>

#include <sstream>

#include "logging/formatter.h"
#include "logging/handler.h"
#include "logging/loglevel.h"
#include "logging/logstyle.h"

using namespace cli::logging;

TEST(HandlerTestIntegration, FileHandlerEmitsCorrectly)
{
    // Use a temporary file name
    std::string tmpFilename = "test_log.txt";
    {
        auto formatter = std::make_unique<MessageOnlyFormatter>();

        FileHandler handler(tmpFilename, std::move(formatter));

        LogRecord infoRecord(LogLevel::INFO, "info-msg");
        LogRecord errorRecord(LogLevel::ERROR, "error-msg");

        handler.emit(infoRecord);
        handler.emit(errorRecord);
    }

    // Read back file contents
    std::ifstream inFile(tmpFilename);
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string contents = buffer.str();

    EXPECT_NE(contents.find("info-msg"), std::string::npos);
    EXPECT_NE(contents.find("error-msg"), std::string::npos);

    // Clean up temporary file
    std::remove(tmpFilename.c_str());
}

TEST(HandlerTestIntegration, FileHandlerThrowsOnBadFile)
{
    auto formatter = std::make_unique<MessageOnlyFormatter>();

    // Choose a filename that's unlikely to be writable
    std::string badFilename = "/invalid/path/log.txt";

    EXPECT_THROW(
        { FileHandler handler(badFilename, std::move(formatter)); }, std::ios_base::failure);
}

TEST(HandlerTestIntegration, FileHandlerDestructorClosesFile)
{
    std::string tmpFilename = "test_log.txt";

    {
        auto formatter = std::make_unique<BasicFormatter>();
        FileHandler handler(tmpFilename, std::move(formatter));

        // Emit a record to ensure file is opened and written
        handler.emit(LogRecord(LogLevel::INFO, "test"));
    }

    // After exiting the scope, handler is destroyed, file should be closed
    std::ofstream testStream(tmpFilename, std::ios::app);
    EXPECT_TRUE(testStream.is_open()); // we can open again => previous one closed
    testStream.close();
}