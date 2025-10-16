#pragma once
#include <memory>
#include <string>
#include <sstream>
#include <functional>
#include "logging/loglevel.h"

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
    explicit LogStreamBuf(std::shared_ptr<std::function<void(LogLevel, const std::string &)>> logFuncPtr,
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
} // namespace cli::logging
