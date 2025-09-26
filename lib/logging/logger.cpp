#include "logger.h"
#include <iostream>

namespace cli::logging
{
    void test()
    {
        std::cout << "Output from logging module" << std::endl;
    }

    void Logger::addHandler(std::unique_ptr<Handler> handler)
    {
        handlers.push_back(std::move(handler));
    }

    void Logger::log(LogLevel lvl, const std::string &msg) const
    {
        if (lvl < minLevel)
            return; // ignore messages below minimum level

        LogRecord record{lvl, msg};

        for (auto const &handler : handlers)
        {
            handler->emit(record);
        }
    }

} // namespace cli::logging