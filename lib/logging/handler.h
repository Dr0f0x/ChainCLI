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
class IHandler
{
public:
    virtual ~IHandler() = default;
    virtual void emit(const LogRecord &record) const = 0;
};

class Handler : public IHandler
{
public:
    Handler(std::ostream &outStream, std::ostream &errStream, std::shared_ptr<IFormatter> f,
            LogLevel minLevel = LogLevel::DEBUG,
            std::shared_ptr<const LogStyleMap> styles = nullptr)
        : out(outStream), err(errStream), formatterPtr(std::move(f)),
          styleMapPtr(std::move(styles)), minLevel(minLevel)
    {
    }

    ~Handler() override;

    void emit(const LogRecord &record) const override;

    void setStylingEnabled(bool enabled) { stylingEnabled = enabled; }

    // Attach a style map (for ANSI colors)
    void setStyleMap(std::shared_ptr<const LogStyleMap> styles);

protected:
    std::ostream &out; // standard stream
    std::ostream &err; // error stream
private:
    bool stylingEnabled{true};
    std::shared_ptr<IFormatter> formatterPtr;
    std::shared_ptr<const LogStyleMap> styleMapPtr;
    LogLevel minLevel;
};

class ConsoleHandler : public Handler
{
public:
    explicit ConsoleHandler(
        std::shared_ptr<IFormatter> f, LogLevel minLevel = LogLevel::DEBUG,
        std::shared_ptr<const LogStyleMap> styles = std::make_shared<LogStyleMap>(defaultStyles()))
        : Handler(std::cout, std::cerr, f, minLevel, std::move(styles))
    {
    }
};

class FileHandler : public Handler
{
public:
    explicit FileHandler(const std::string &filename, std::shared_ptr<IFormatter> f,
                         LogLevel minLevel = LogLevel::DEBUG,
                         std::shared_ptr<const LogStyleMap> styles = nullptr);
    ~FileHandler() override;

private:
    std::ofstream file;
};

} // namespace cli::logging
