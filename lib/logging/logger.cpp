#include <utility>

#include "logger.h"

namespace cli::logging
{
    Logger::Logger(LogLevel lvl)
        : minLevel(lvl)
    {
        // Wrap logInternal as a lambda and pass it to LogStreamBuf
        auto logFuncPtr = std::make_shared<std::function<void(LogLevel, const std::string &)>>(
            [this](LogLevel level, const std::string &msg)
            { this->logInternal(level, msg); });

        for (int i = std::to_underlying(LogLevel::TRACE); i <= std::to_underlying(LogLevel::ERROR); ++i)
        {
            auto level = static_cast<LogLevel>(i);
            buffers[level] = std::make_unique<LogStreamBuf>(logFuncPtr, level, minLevel);
            streams[level] = std::make_unique<std::ostream>(buffers[level].get());
        }
    }

    void Logger::setLevel(LogLevel lvl)
    {
        minLevel = lvl;
        for (auto const &[level, buffer] : buffers)
        {
            buffer->setMinLevel(lvl);
        }
    }

    void Logger::addHandler(std::unique_ptr<IHandler> handlerPtr)
    {
        handlers.push_back(std::move(handlerPtr));
    }

    void Logger::logInternal(LogLevel lvl, const std::string &msg) const
    {
        LogRecord record{lvl, msg};

        for (auto const &handler : handlers)
        {
            handler->emit(record);
        }
    }

    int LogStreamBuf::sync()
    {
        if (lvl < minLevel)
            return 0; // skip

        if (auto msg = str(); !msg.empty())
        {
            (*logFuncPtr)(lvl, msg); // call the function
            str("");                 // clear the buffer
        }
        return 0;
    }

} // namespace cli::logging