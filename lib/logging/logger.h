#pragma once
#include "handler.h"
#include <vector>
#include <memory>

namespace cli::logging
{
    void test();

    // Logger interface
    class ILogger
    {
    public:
        virtual ~ILogger() = default;

        // Set the minimum logging level
        virtual void setLevel(LogLevel lvl) = 0;

        // Add a handler
        virtual void addHandler(std::unique_ptr<Handler> handler) = 0;
        virtual void removeAllHandlers() = 0;

        // Core logging function
        virtual void log(LogLevel lvl, const std::string &msg) const = 0;

        // Convenience helpers
        virtual void trace(const std::string &msg) const = 0;
        virtual void debug(const std::string &msg) const = 0;
        virtual void detail(const std::string &msg) const = 0;
        virtual void info(const std::string &msg) const = 0;
        virtual void warning(const std::string &msg) const = 0;
        virtual void error(const std::string &msg) const = 0;
    };

    class Logger : public ILogger
    {
    public:
        // Non-copyable
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        Logger(Logger &&) = default;
        Logger &operator=(Logger &&) = default;

        explicit Logger(LogLevel lvl = LogLevel::TRACE) : minLevel(lvl) {}

        void setLevel(LogLevel lvl) override { minLevel = lvl; }

        void addHandler(std::unique_ptr<Handler> handler) override;
        void removeAllHandlers() override { handlers.clear(); }

        void log(LogLevel lvl, const std::string &msg) const override;

        // Convenience helpers
        void trace(const std::string &msg) const override { log(LogLevel::TRACE, msg); }
        void debug(const std::string &msg) const override { log(LogLevel::DEBUG, msg); }
        void detail(const std::string &msg) const override { log(LogLevel::DETAIL, msg); }
        void info(const std::string &msg) const override { log(LogLevel::INFO, msg); }
        void warning(const std::string &msg) const override { log(LogLevel::WARNING, msg); }
        void error(const std::string &msg) const override { log(LogLevel::ERROR, msg); }

    private:
        LogLevel minLevel;
        std::vector<std::unique_ptr<Handler>> handlers;
    };
}