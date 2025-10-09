/*
 * Copyright 2025 Dominik Czekai
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "handler.h"

namespace cli::logging
{

/// @brief Log stream buffer with a minimum LogLevel, that redirects the buffered output to a
/// logging function. Used to offer own streams to write to for each log level.
/// @note Does not flush automatically on newline, call sync() or explicitly flush buffer.
class LogStreamBuf : public std::stringbuf
{
public:
    /// @brief Construct a new LogStreamBuf
    /// @param logFuncPtr The logging function to call with the buffered output
    /// @param lvl The log level for this buffer
    /// @param lvlMin The minimum log level for this buffer
    LogStreamBuf(std::shared_ptr<std::function<void(LogLevel, const std::string &)>> logFuncPtr,
                 LogLevel lvl, LogLevel lvlMin)
        : logFuncPtr(logFuncPtr), lvl(lvl), minLevel(lvlMin)
    {
    }

    int sync() override;

    /// @brief Set the minimum log level for this buffer
    /// @param lvlMin The new minimum log level
    void setMinLevel(LogLevel lvlMin) { minLevel = lvlMin; }

private:
    std::shared_ptr<std::function<void(LogLevel, const std::string &)>> logFuncPtr;
    LogLevel lvl;
    LogLevel minLevel;
};

/// @brief Logger class for handling log messages.
class Logger
{
public:
    // Non-copyable
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    Logger(Logger &&) = default;
    Logger &operator=(Logger &&) = default;

    /// @brief Construct a new Logger object with the specified minimum log level.
    /// @param lvl The minimum log level for this logger
    explicit Logger(LogLevel lvl = LogLevel::TRACE);

    /// @brief Set the minimum log level for this logger.
    /// @param lvl The new minimum log level
    void setLevel(LogLevel lvl);

    /// @brief Add a log handler.
    /// @param handlerPtr The log handler to add
    void addHandler(std::unique_ptr<AbstractHandler> handlerPtr);

    /// @brief Remove all log handlers.
    void removeAllHandlers() { handlers.clear(); }

    /// @brief Log a message at the specified log level using a format string to print the passed
    /// arguments.
    /// @tparam ...Args The argument types for the format string
    /// @param lvl The log level for this message
    /// @param fmt The format string
    /// @param ...args The arguments for the format string
    template <typename... Args> void log(LogLevel lvl, const std::string &fmt, Args &&...args) const
    {
        if (lvl < minLevel)
            return;
        std::string formatted = std::vformat(fmt, std::make_format_args(args...));
        logInternal(lvl, formatted);
    }

#pragma region LogLevelShortcuts

    /// @brief Log a message at the TRACE level.
    /// @tparam ...Args The argument types for the format string
    /// @param fmt The format string
    /// @param ...args The arguments for the format string
    template <typename... Args> void trace(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::TRACE, fmt, std::forward<Args>(args)...);
    }

    /// @brief Log a message at the VERBOSE level.
    /// @tparam ...Args The argument types for the format string
    /// @param fmt The format string
    /// @param ...args The arguments for the format string
    template <typename... Args> void verbose(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::VERBOSE, fmt, std::forward<Args>(args)...);
    }

    /// @brief Log a message at the DEBUG level.
    /// @tparam ...Args The argument types for the format string
    /// @param fmt The format string
    /// @param ...args The arguments for the format string
    template <typename... Args> void debug(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::DEBUG, fmt, std::forward<Args>(args)...);
    }

    /// @brief Log a message at the SUCCESS level.
    /// @tparam ...Args The argument types for the format string
    /// @param fmt The format string
    /// @param ...args The arguments for the format string
    template <typename... Args> void success(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::SUCCESS, fmt, std::forward<Args>(args)...);
    }

    /// @brief Log a message at the INFO level.
    /// @tparam ...Args The argument types for the format string
    /// @param fmt The format string
    /// @param ...args The arguments for the format string
    template <typename... Args> void info(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::INFO, fmt, std::forward<Args>(args)...);
    }

    /// @brief Log a message at the WARNING level.
    /// @tparam ...Args The argument types for the format string
    /// @param fmt The format string
    /// @param ...args The arguments for the format string
    template <typename... Args> void warning(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::WARNING, fmt, std::forward<Args>(args)...);
    }

    /// @brief Log a message at the ERROR level.
    /// @tparam ...Args The argument types for the format string
    /// @param fmt The format string
    /// @param ...args The arguments for the format string
    template <typename... Args> void error(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::ERROR, fmt, std::forward<Args>(args)...);
    }

#pragma endregion LogLevelShortcuts

#pragma region LogStreamShortcuts

    /// @brief Get the stream for the specified log level.
    /// @param lvl The log level
    /// @return The output stream for the specified log level
    std::ostream &getStream(LogLevel lvl) { return *streams[lvl]; }

    /// @brief Get the stream for the TRACE log level.
    /// @return The output stream for the TRACE log level
    std::ostream &trace() { return *streams[LogLevel::TRACE]; }

    /// @brief Get the stream for the VERBOSE log level.
    /// @return The output stream for the VERBOSE log level
    std::ostream &verbose() { return *streams[LogLevel::VERBOSE]; }

    /// @brief Get the stream for the DEBUG log level.
    /// @return The output stream for the DEBUG log level
    std::ostream &debug() { return *streams[LogLevel::DEBUG]; }

    /// @brief Get the stream for the SUCCESS log level.
    /// @return The output stream for the SUCCESS log level
    std::ostream &success() { return *streams[LogLevel::SUCCESS]; }

    /// @brief Get the stream for the INFO log level.
    /// @return The output stream for the INFO log level
    std::ostream &info() { return *streams[LogLevel::INFO]; }

    /// @brief Get the stream for the WARNING log level.
    /// @return The output stream for the WARNING log level
    std::ostream &warning() { return *streams[LogLevel::WARNING]; }

    /// @brief Get the stream for the ERROR log level.
    /// @return The output stream for the ERROR log level
    std::ostream &error() { return *streams[LogLevel::ERROR]; }

#pragma endregion LogStreamShortcuts

private:
    void logInternal(LogLevel lvl, const std::string &fmt) const;
    LogLevel minLevel;
    std::vector<std::unique_ptr<AbstractHandler>> handlers;

    // Per-level stream buffers & streams
    std::unordered_map<LogLevel, std::unique_ptr<LogStreamBuf>> buffers;
    std::unordered_map<LogLevel, std::unique_ptr<std::ostream>> streams;
};
} // namespace cli::logging