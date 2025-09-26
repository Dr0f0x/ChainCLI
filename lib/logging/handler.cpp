#include "handler.h"

namespace cli::logging
{

    void Handler::emit(const LogRecord &record)
    {
        if (record.level < minLevel)
            return; // ignore messages below minimum level

        auto formatted = formatter->format(record);

        if (stylingEnabled && styleMap)
        {
            auto it = styleMap->find(record.level);
            if (it != styleMap->end())
            {
                formatted = it->second + formatted + "\o{33}[0m"; // reset style
            }
        }

        if (record.level >= LogLevel::ERROR)
        {
            err << formatted << '\n';
        }
        else
        {
            out << formatted << '\n';
        }
    }

    void Handler::setStyleMap(std::shared_ptr<const LogStyleMap> styles)
    {
        styleMap = styles;
    }

    FileHandler::FileHandler(const std::string &filename, std::unique_ptr<Formatter> f, LogLevel minLevel, std::shared_ptr<const LogStyleMap> styles)
        : Handler(file, file, std::move(f), minLevel, std::move(styles)), file(filename, std::ios::app)
    {
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open log file: " + filename);
        }
    }
} // namespace cli::logging