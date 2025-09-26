#pragma once
#include "formatter.h"
#include <iostream>
#include <fstream>
#include <memory>

namespace cli::logging
{

    class Handler
    {
    public:
        Handler(std::ostream& outStream,
            std::ostream& errStream,
            std::unique_ptr<Formatter> f,
            LogLevel minLevel = LogLevel::DEBUG)
        : out(outStream), err(errStream), formatter(std::move(f)), minLevel(minLevel) {}

        virtual ~Handler() = default;

        void emit(const LogRecord &record);

    protected:
        std::ostream &out; // standard stream
        std::ostream &err; // error stream
    private:
        std::unique_ptr<Formatter> formatter;
        LogLevel minLevel;
    };

    class ConsoleHandler : public Handler
    {
    public:
        explicit ConsoleHandler(std::unique_ptr<Formatter> f, LogLevel minLevel = LogLevel::DEBUG)
        : Handler(std::cout, std::cerr, std::move(f), minLevel) {}
    };

    class FileHandler : public Handler
    {
    public:
        explicit FileHandler(const std::string& filename,
                std::unique_ptr<Formatter> f,
                LogLevel minLevel = LogLevel::DEBUG);;
        ~FileHandler() override = default;

    private:
        std::ofstream file;
    };

} // namespace cli::logging
