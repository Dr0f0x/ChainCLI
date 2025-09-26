#include "logger.h"

namespace cli::logging
{
    void Logger::addHandler(std::unique_ptr<Handler> handler)
    {
        handlers.push_back(std::move(handler));
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