#pragma once
#include "formatter.h"
#include "logstyle.h"
#include <iostream>
#include <fstream>
#include <memory>

namespace cli::logging
{

    class Handler
    {
    public:
        Handler(std::ostream &outStream,
                std::ostream &errStream,
                std::unique_ptr<Formatter> f,
                LogLevel minLevel = LogLevel::DEBUG,
                std::shared_ptr<const LogStyleMap> styles = nullptr)
            : out(outStream), err(errStream), formatter(std::move(f)), styleMap(std::move(styles)), minLevel(minLevel) {}

        virtual ~Handler() = default;

        void emit(const LogRecord &record);
        void setStylingEnabled(bool enabled) { stylingEnabled = enabled; }

        // Attach a style map (for ANSI colors)
        void setStyleMap(std::shared_ptr<const LogStyleMap> styles);

    protected:
        std::ostream &out; // standard stream
        std::ostream &err; // error stream
    private:
        bool stylingEnabled{true};
        std::unique_ptr<Formatter> formatter;
        std::shared_ptr<const LogStyleMap> styleMap;
        LogLevel minLevel;
    };

    class ConsoleHandler : public Handler
    {
    public:
        explicit ConsoleHandler(std::unique_ptr<Formatter> f,
                LogLevel minLevel = LogLevel::DEBUG,
                std::shared_ptr<const LogStyleMap> styles = std::make_shared<LogStyleMap>(defaultStyles()))
            : Handler(std::cout, std::cerr, std::move(f), minLevel, std::move(styles)) {}
    };

    class FileHandler : public Handler
    {
    public:
        explicit FileHandler(const std::string &filename,
            std::unique_ptr<Formatter> f,
            LogLevel minLevel = LogLevel::DEBUG,
            std::shared_ptr<const LogStyleMap> styles = nullptr);
        ;
        ~FileHandler() override = default;

    private:
        std::ofstream file;
    };

} // namespace cli::logging
