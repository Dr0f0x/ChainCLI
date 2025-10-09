// Copyright 2025 Dominik Czekai
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "logger.h"

#include <utility>

namespace cli::logging
{
Logger::Logger(LogLevel lvl) : minLevel(lvl)
{
    // Wrap logInternal as a lambda and pass it to LogStreamBuf
    auto logFuncPtr = std::make_shared<std::function<void(LogLevel, const std::string &)>>(
        [this](LogLevel level, const std::string &msg) { this->logInternal(level, msg); });

    for (int i = std::to_underlying(LogLevel::TRACE); i <= std::to_underlying(LogLevel::ERROR); ++i)
    {
        auto level = static_cast<LogLevel>(i);
        buffers[level] = std::make_unique<LogStreamBuf>(logFuncPtr, level, minLevel);
        streams[level] = std::make_unique<std::ostream>(buffers[level].get());
    }
}

void Logger::setLevel(LogLevel lvl)
{
    minLevel = lvl;
    for (auto const &[level, buffer] : buffers)
    {
        buffer->setMinLevel(lvl);
    }
}

void Logger::addHandler(std::unique_ptr<AbstractHandler> handlerPtr)
{
    handlers.push_back(std::move(handlerPtr));
}

void Logger::logInternal(LogLevel lvl, const std::string &msg) const
{
    LogRecord record{lvl, msg};

    for (auto const &handler : handlers)
    {
        handler->emit(record);
    }
}

int LogStreamBuf::sync()
{
    if (lvl < minLevel)
        return 0; // skip

    if (auto msg = str(); !msg.empty())
    {
        (*logFuncPtr)(lvl, msg); // call the function
        str("");                 // clear the buffer
    }
    return 0;
}

} // namespace cli::logging