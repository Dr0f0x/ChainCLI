#include "handler.h"

namespace cli::logging
{

    void Handler::emit(const LogRecord &record)
    {
        if (record.level < minLevel)
            return; // ignore messages below minimum level

        auto formatted = formatter->format(record);

        if (record.level >= LogLevel::ERROR) {
            err << formatted << '\n';
        } else {
            out << formatted << '\n';
        }
    }

    FileHandler::FileHandler(const std::string &filename,
                             std::unique_ptr<Formatter> f,
                             LogLevel minLevel)
        : Handler(file, file, std::move(f), minLevel),
          file(filename, std::ios::app)
    {
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open log file: " + filename);
        }
    }
} // namespace cli::logging