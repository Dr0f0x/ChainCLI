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
#include <fstream>
#include <iostream>
#include <memory>

#include "formatter.h"
#include "logstyle.h"

namespace cli::logging
{

/// @brief Interface for log record handlers.
class AbstractHandler
{
public:
    virtual ~AbstractHandler() = default;

    /// @brief Emit a log record, using the handlers formatter and specified output.
    /// @param record The log record to emit.
    virtual void emit(const LogRecord &record) const = 0;
};

/// @brief Basic log handler that writes to specified output streams.
class BaseHandler : public AbstractHandler
{
public:
    /// @brief Construct a new Base Handler using the given out and error streams.
    /// @param outStream The output stream for log messages
    /// @param errStream The error stream for log messages
    /// @param formatter The formatter to use for log messages
    /// @param minLevel The minimum log level for this handler
    /// @param styles The styles to use for log messages
    BaseHandler(std::ostream &outStream, std::ostream &errStream,
                std::shared_ptr<AbstractFormatter> formatter, LogLevel minLevel = LogLevel::DEBUG,
                std::shared_ptr<const LogStyleMap> styles = nullptr)
        : out(outStream), err(errStream), formatterPtr(std::move(formatter)),
          styleMapPtr(std::move(styles)), minLevel(minLevel)
    {
    }

    ~BaseHandler() override;

    void emit(const LogRecord &record) const override;

    /// @brief Enable or disable styling for log messages.
    /// @param enabled Whether styling should be enabled.
    void setStylingEnabled(bool enabled) { stylingEnabled = enabled; }

    /// @brief Attach a style map (for ANSI colors).
    /// @param styles The styles to use for log messages.
    void setStyleMap(std::shared_ptr<const LogStyleMap> styles);

protected:
    std::ostream &out; // standard stream
    std::ostream &err; // error stream
private:
    bool stylingEnabled{true};
    std::shared_ptr<AbstractFormatter> formatterPtr;
    std::shared_ptr<const LogStyleMap> styleMapPtr;
    LogLevel minLevel;
};

/// @brief Console log handler.
class ConsoleHandler : public BaseHandler
{
public:
    /// @brief Construct a new Console Handler that uses std::cout and std::cerr.
    /// @param formatter The formatter to use for log messages
    /// @param minLevel The minimum log level for this handler
    /// @param styles The styles to use for log messages
    explicit ConsoleHandler(
        std::shared_ptr<AbstractFormatter> formatter, LogLevel minLevel = LogLevel::DEBUG,
        std::shared_ptr<const LogStyleMap> styles = std::make_shared<LogStyleMap>(defaultStyles()))
        : BaseHandler(std::cout, std::cerr, formatter, minLevel, std::move(styles))
    {
    }
};

/// @brief File log handler.
class FileHandler : public BaseHandler
{
public:
    /// @brief Construct a new File Handler that writes to the specified file.
    /// @param filename The name of the file to write to
    /// @param formatter The formatter to use for log messages
    /// @param minLevel The minimum log level for this handler
    /// @param styles The styles to use for log messages
    explicit FileHandler(const std::string &filename, std::shared_ptr<AbstractFormatter> formatter,
                         LogLevel minLevel = LogLevel::DEBUG,
                         std::shared_ptr<const LogStyleMap> styles = nullptr);
    ~FileHandler() override;

private:
    std::ofstream file;
};

} // namespace cli::logging
