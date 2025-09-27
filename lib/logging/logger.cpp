#include "logger.h"

namespace cli::logging
{
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

} // namespace cli::logging