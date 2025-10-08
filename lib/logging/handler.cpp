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

#include "handler.h"

namespace cli::logging
{
Handler::~Handler()
{
    err.flush();
    out.flush();
}

void Handler::emit(const LogRecord &record) const
{
    if (record.level < minLevel)
        return; // ignore messages below minimum level

    auto formatted = formatterPtr->format(record);

    if (stylingEnabled && styleMapPtr)
    {
        auto it = styleMapPtr->find(record.level);
        if (it != styleMapPtr->end())
        {
            formatted = it->second + formatted + "\o{33}[0m"; // reset style
        }
    }

    if (record.level >= LogLevel::ERROR)
    {
        err << formatted;
    }
    else
    {
        out << formatted;
    }
}

void Handler::setStyleMap(std::shared_ptr<const LogStyleMap> styles)
{
    styleMapPtr = styles;
}

FileHandler::FileHandler(const std::string &filename, std::shared_ptr<IFormatter> f,
                         LogLevel minLevel, std::shared_ptr<const LogStyleMap> styles)
    : Handler(file, file, f, minLevel, std::move(styles)), file(filename, std::ios::app)
{
    if (!file.is_open())
    {
        throw std::ios_base::failure("Failed to open log file: " + filename);
    }
}

FileHandler::~FileHandler()
{
    if (file.is_open())
    {
        file.close(); // ensure the file is closed
    }
}
} // namespace cli::logging