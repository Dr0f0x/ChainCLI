#include "logger.h"
#include <iostream>

namespace cli::logging
{
    void test()
    {
        std::cout << "Output from logging module" << std::endl;
    }

    void Logger::log(LogLevel lvl, const std::string &msg)
    {
        if (lvl < minLevel)
            return; // ignore messages below minimum level

        LogRecord record{lvl, msg};

        for (auto &handler : handlers)
        {
            handler->emit(record);
        }
    }

} // namespace cli::logging