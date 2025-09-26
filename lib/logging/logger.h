#pragma once
#include "handler.h"
#include <vector>
#include <memory>

namespace cli::logging
{
    void test();

    class Logger
    {
    public:
        explicit Logger(LogLevel lvl = LogLevel::TRACE) : minLevel(lvl) {}

        void setLevel(LogLevel lvl) { minLevel = lvl; }

        void addHandler(std::unique_ptr<Handler> handler)
        {
            handlers.push_back(std::move(handler));
        }

        void log(LogLevel lvl, const std::string &msg);

        // Convenience helpers
        void trace(const std::string &msg) { log(LogLevel::TRACE, msg); }
        void debug(const std::string &msg) { log(LogLevel::DEBUG, msg); }
        void detail(const std::string &msg) { log(LogLevel::DETAIL, msg); }
        void info(const std::string &msg) { log(LogLevel::INFO, msg); }
        void warning(const std::string &msg) { log(LogLevel::WARNING, msg); }
        void error(const std::string &msg) { log(LogLevel::ERROR, msg); }

    private:
        LogLevel minLevel;
        std::vector<std::unique_ptr<Handler>> handlers;
    };
}