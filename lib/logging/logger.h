#pragma once
#include "handler.h"
#include <vector>
#include <memory>

namespace cli::logging
{
    class Logger
    {
    public:
        // Non-copyable
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        Logger(Logger &&) = default;
        Logger &operator=(Logger &&) = default;

        explicit Logger(LogLevel lvl = LogLevel::TRACE) : minLevel(lvl) {}

        void setLevel(LogLevel lvl) { minLevel = lvl; }

        void addHandler(std::unique_ptr<Handler> handler);
        void removeAllHandlers() { handlers.clear(); }

        template <typename... Args>
        void log(LogLevel lvl, const std::string &fmt, Args &&...args) const
        {
            if (lvl < minLevel)
                return;
            std::string formatted = std::vformat(fmt, std::make_format_args(std::forward<Args>(args)...));
            logInternal(lvl, formatted);
        }

        // Convenience helpers
        template <typename... Args>
        void trace(const std::string &fmt, Args &&...args) const { log(LogLevel::TRACE, fmt, std::forward<Args>(args)...); }
        template <typename... Args>
        void verbose(const std::string &fmt, Args &&...args) const { log(LogLevel::VERBOSE, fmt, std::forward<Args>(args)...); }
        template <typename... Args>
        void debug(const std::string &fmt, Args &&...args) const { log(LogLevel::DEBUG, fmt, std::forward<Args>(args)...); }
        template <typename... Args>
        void detail(const std::string &fmt, Args &&...args) const { log(LogLevel::DETAIL, fmt, std::forward<Args>(args)...); }
        template <typename... Args>
        void info(const std::string &fmt, Args &&...args) const { log(LogLevel::INFO, fmt, std::forward<Args>(args)...); }
        template <typename... Args>
        void warning(const std::string &fmt, Args &&...args) const { log(LogLevel::WARNING, fmt, std::forward<Args>(args)...); }
        template <typename... Args>
        void error(const std::string &fmt, Args &&...args) const { log(LogLevel::ERROR, fmt, std::forward<Args>(args)...); }

    private:
        void logInternal(LogLevel lvl, const std::string &fmt) const;
        LogLevel minLevel;
        std::vector<std::unique_ptr<Handler>> handlers;
    };
}