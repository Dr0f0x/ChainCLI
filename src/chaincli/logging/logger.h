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
#include "log_streambuffer.h"

namespace cli::logging
{

/// @brief Abstract base class for logger implementations
class AbstractLogger
{
public:
    virtual ~AbstractLogger() = default;

    /// @brief Set the minimum log level for this logger.
    /// @param lvl The new minimum log level
    virtual void setLevel(LogLevel lvl) = 0;

    /// @brief Add a log handler.
    /// @param handlerPtr The log handler to add
    virtual void addHandler(std::unique_ptr<AbstractHandler> handlerPtr) = 0;

    /// @brief Remove all log handlers.
    virtual void removeAllHandlers() = 0;

    /// @brief Log a message at the specified log level.
    /// @param lvl The log level
    /// @param message The message to log
    virtual void log(LogLevel lvl, const std::string &message) const = 0;

    /// @brief Get the stream for the specified log level.
    /// @param lvl The log level
    /// @return The output stream for the specified log level
    virtual std::ostream &getStream(LogLevel lvl) = 0;

    /// @brief Log a message at the specified log level using a format string to print the passed
    /// arguments.
    /// @tparam ...Args The argument types for the format string
    /// @param lvl The log level for this message
    /// @param fmt The format string
    /// @param ...args The arguments for the format string
    template <typename... Args> void log(LogLevel lvl, const std::string &fmt, Args &&...args) const
    {
        std::string formatted = std::vformat(fmt, std::make_format_args(args...));
        log(lvl, formatted);
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

    /// @brief Get the stream for the TRACE log level.
    /// @return The output stream for the TRACE log level
    std::ostream &trace() { return getStream(LogLevel::TRACE); }

    /// @brief Get the stream for the VERBOSE log level.
    /// @return The output stream for the VERBOSE log level
    std::ostream &verbose() { return getStream(LogLevel::VERBOSE); }

    /// @brief Get the stream for the DEBUG log level.
    /// @return The output stream for the DEBUG log level
    std::ostream &debug() { return getStream(LogLevel::DEBUG); }

    /// @brief Get the stream for the SUCCESS log level.
    /// @return The output stream for the SUCCESS log level
    std::ostream &success() { return getStream(LogLevel::SUCCESS); }

    /// @brief Get the stream for the INFO log level.
    /// @return The output stream for the INFO log level
    std::ostream &info() { return getStream(LogLevel::INFO); }

    /// @brief Get the stream for the WARNING log level.
    /// @return The output stream for the WARNING log level
    std::ostream &warning() { return getStream(LogLevel::WARNING); }

    /// @brief Get the stream for the ERROR log level.
    /// @return The output stream for the ERROR log level
    std::ostream &error() { return getStream(LogLevel::ERROR); }

#pragma endregion LogStreamShortcuts
};

/// @brief Logger class for handling log messages.
class Logger : public AbstractLogger
{
public:
    ~Logger() override = default;
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
    void setLevel(LogLevel lvl) override;

    /// @brief Add a log handler.
    /// @param handlerPtr The log handler to add
    void addHandler(std::unique_ptr<AbstractHandler> handlerPtr) override;

    /// @brief Remove all log handlers.
    void removeAllHandlers() override { handlers.clear(); }

    void log(LogLevel lvl, const std::string& msg) const override;

    std::ostream& getStream(LogLevel lvl) override;

private:
    LogLevel minLevel;
    std::vector<std::unique_ptr<AbstractHandler>> handlers;

    // Per-level stream buffers & streams
    std::unordered_map<LogLevel, std::unique_ptr<LogStreamBuf>> buffers;
    std::unordered_map<LogLevel, std::unique_ptr<std::ostream>> streams;
};
} // namespace cli::logging