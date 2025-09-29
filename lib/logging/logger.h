#pragma once
#include "handler.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace cli::logging
{
    class LogStreamBuf : public std::stringbuf
    {
    public:
        LogStreamBuf(std::shared_ptr<std::function<void(LogLevel, const std::string &)>> logFuncPtr, LogLevel lvl, LogLevel lvlMin)
            : logFuncPtr(logFuncPtr), lvl(lvl), minLevel(lvlMin) {}
        int sync() override;
        
        void setMinLevel(LogLevel lvlMin) { minLevel = lvlMin; }

    private:
        std::shared_ptr<std::function<void(LogLevel, const std::string &)>> logFuncPtr;
        LogLevel lvl;
        LogLevel minLevel;
    };

    class Logger
    {
    public:
        // Non-copyable
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        Logger(Logger &&) = default;
        Logger &operator=(Logger &&) = default;

        explicit Logger(LogLevel lvl = LogLevel::TRACE);

        void setLevel(LogLevel lvl);

        void addHandler(std::unique_ptr<IHandler> handlerPtr);
        void removeAllHandlers() { handlers.clear(); }

        template <typename... Args>
        void log(LogLevel lvl, const std::string &fmt, Args &&...args) const
        {
            if (lvl < minLevel)
                return;
            std::string formatted = std::vformat(fmt, std::make_format_args(args...));
            logInternal(lvl, formatted);
        }

        // Convenience helpers
        template <typename... Args>
        void trace(const std::string &fmt, Args &&...args) { log(LogLevel::TRACE, fmt, std::forward<Args>(args)...); }
        template <typename... Args>
        void verbose(const std::string &fmt, Args &&...args) { log(LogLevel::VERBOSE, fmt, std::forward<Args>(args)...); }
        template <typename... Args>
        void debug(const std::string &fmt, Args &&...args) { log(LogLevel::DEBUG, fmt, std::forward<Args>(args)...); }
        template <typename... Args>
        void success(const std::string &fmt, Args &&...args) { log(LogLevel::SUCCESS, fmt, std::forward<Args>(args)...); }
        template <typename... Args>
        void info(const std::string &fmt, Args &&...args) { log(LogLevel::INFO, fmt, std::forward<Args>(args)...); }
        template <typename... Args>
        void warning(const std::string &fmt, Args &&...args) { log(LogLevel::WARNING, fmt, std::forward<Args>(args)...); }
        template <typename... Args>
        void error(const std::string &fmt, Args &&...args) { log(LogLevel::ERROR, fmt, std::forward<Args>(args)...); }

        std::ostream &getStream(LogLevel lvl)
        {
            return *streams[lvl];
        }

        std::ostream &trace() { return *streams[LogLevel::TRACE]; }
        std::ostream &verbose() { return *streams[LogLevel::VERBOSE]; }
        std::ostream &debug() { return *streams[LogLevel::DEBUG]; }
        std::ostream &success() { return *streams[LogLevel::SUCCESS]; }
        std::ostream &info() { return *streams[LogLevel::INFO]; }
        std::ostream &warning() { return *streams[LogLevel::WARNING]; }
        std::ostream &error() { return *streams[LogLevel::ERROR]; }

    private:
        void logInternal(LogLevel lvl, const std::string &fmt) const;
        LogLevel minLevel;
        std::vector<std::unique_ptr<IHandler>> handlers;

        // Per-level stream buffers & streams
        std::unordered_map<LogLevel, std::unique_ptr<LogStreamBuf>> buffers;
        std::unordered_map<LogLevel, std::unique_ptr<std::ostream>> streams;
    };
}