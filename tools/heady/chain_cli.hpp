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

#include <algorithm>
#include <any>
#include <chrono>
#include <format>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace cli
{
/// @brief Holds the configuration for the CLI application
struct CliConfig
{
    // General CLI metadata
    std::string title;
    std::string executableName; // e.g. argv[0]
    std::string description;    // human-readable description
    std::string version;        // e.g. "1.2.3"

    // Runtime flags and options
    char repeatableDelimiter{','};
    int optionsWidth{20}; // width that is used to right aling the options text for arguments

    // Behavior toggles
    // ...
};

} // namespace cli

namespace cli::commands
{

/// @brief Enumeration of argument kinds.
enum class ArgumentKind
{
    Positional,
    Option,
    Flag,
};

namespace docwriting
{
class DocWriter; // Forward declaration
}

/// @brief Base class for command-line arguments.
class ArgumentBase
{
public:
    virtual ~ArgumentBase() = default;

#pragma region Accessors

    /// @brief Get the name of the argument.
    /// @details The name is a unique identifier for the argument within its command.
    /// @return The name of the argument.
    [[nodiscard]] constexpr std::string_view getName() const noexcept { return name; }

    /// @brief Get the option comment for the argument.
    /// @details The option comment is a brief description of the argument's purpose, used in
    /// help messages and documentation.
    /// @return The option comment for the argument.
    [[nodiscard]] constexpr std::string_view getOptionComment() const noexcept
    {
        return optionsComment;
    }

    /// @brief Check if the argument is required.
    /// @return True if the argument is required, false otherwise.
    [[nodiscard]] constexpr bool isRequired() const noexcept { return required; }

    /// @brief Check if the argument is repeatable.
    /// @return True if the argument is repeatable, false otherwise.
    [[nodiscard]] constexpr bool isRepeatable() const noexcept { return repeatable; }

    /// @brief Get the type of the argument.
    /// @return The type of the argument.
    [[nodiscard]] constexpr ArgumentKind getArgType() const { return argType; }

    /// @brief Get the options documentation string for the argument.
    /// @details This string is used in the Options section of the help message for a command, where
    /// each argument is listed with its description.
    /// @param writer The documentation writer to use.
    /// @return The options documentation string for the argument.
    [[nodiscard]] virtual std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const = 0;

    /// @brief Get the argument documentation string for the argument.
    /// @details This string is used in the Usage section of the help message for a command and
    /// displays a textual representation of the arguments attributes (e.g. if it is required or
    /// repeatable, ...).
    /// @param writer The documentation writer to use.
    /// @return The argument documentation string for the argument.
    [[nodiscard]] virtual std::string getArgDocString(
        const docwriting::DocWriter &writer) const = 0;
#pragma endregion Accessors

protected:
    ArgumentBase(std::string_view name, std::string_view optionsComment, ArgumentKind argType,
                 bool repeatable, bool required)
        : name(name), optionsComment(optionsComment), argType(argType), repeatable(repeatable),
          required(required)
    {
    }

    const std::string name;
    std::string optionsComment;
    const ArgumentKind argType;
    bool repeatable{false};
    bool required{true};
};

/// @brief Base class for typed command-line arguments.
class TypedArgumentBase
{
public:
    virtual ~TypedArgumentBase() = default;

    /// @brief Get the type of the argument.
    /// @return The type of the argument.
    [[nodiscard]] std::type_index getType() const { return type; }

    /// @brief Parse the input string to the argument's value type.
    /// @param input The input string to parse.
    /// @return The parsed value as std::any.
    [[nodiscard]] virtual std::any parseToValue(const std::string &input) const = 0;

protected:
    explicit TypedArgumentBase(std::type_index t) : type(t) {}

    const std::type_index type;
};

/// @brief Base class for flag-like command-line arguments.
class FlaggedArgumentBase
{
public:
    /// @brief Get the short name of the argument.
    /// @return The short name of the argument.
    [[nodiscard]] constexpr std::string_view getShortName() const noexcept { return shortName; }

protected:
    explicit FlaggedArgumentBase(std::string_view shortName) : shortName(shortName) {}

    std::string shortName;
};

} // namespace cli::commands

namespace cli::commands
{

/// @brief Base class for command-line argument groups.
class ArgumentGroup
{
public:
    /// @brief Construct a new ArgumentGroup.
    /// @tparam ...Args The types of the arguments to include in the group.
    /// @param exclusive Whether the group is exclusive.
    /// @param inclusive Whether the group is inclusive.
    /// @param ...args The arguments to include in the group.
    template <typename... Args>
        requires((!std::same_as<std::remove_cvref_t<Args>, ArgumentGroup> && ...) &&
                 (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
    explicit ArgumentGroup(bool exclusive, bool inclusive, Args &&...args)
        : exclusive(exclusive), inclusive(inclusive)
    {
        (arguments.emplace_back(
             std::make_shared<std::remove_cvref_t<Args>>(std::forward<Args>(args))),
         ...);
    }

    virtual ~ArgumentGroup() = default;

    /// @brief Get the arguments in the group.
    /// @details The arguments are returned in the order they were added to the group.
    /// @return A const reference to the vector of arguments in the group.
    [[nodiscard]] const std::vector<std::shared_ptr<ArgumentBase>> &getArguments() const noexcept
    {
        return arguments;
    }

    /// @brief Check if the group is exclusive.
    /// @details An exclusive group means that only one argument in the group can be provided by
    /// the user.
    /// @return True if the group is exclusive, false otherwise.
    [[nodiscard]] bool isExclusive() const noexcept { return exclusive; }

    /// @brief Check if the group is inclusive.
    /// @details An inclusive group means that at least one argument in the group must be provided
    /// by the user.
    /// @return True if the group is inclusive, false otherwise.
    [[nodiscard]] bool isInclusive() const noexcept { return inclusive; }

    /// @brief Check if this group is required.
    /// @details A required group means that for exclusive groups, one of argument in the group must
    /// be provided, while for inclusive groups, all arguments must be provided.
    /// @return True if the group is required, false otherwise.
    bool isRequired() const;

    /// @brief Add an argument to the group.
    /// @param arg The argument to add.
    void addArgument(const std::shared_ptr<ArgumentBase> &arg) { arguments.push_back(arg); }

private:
    std::vector<std::shared_ptr<ArgumentBase>> arguments;
    bool exclusive;
    bool inclusive;
};

/// @brief Exclusive argument group.
class ExclusiveGroup : public ArgumentGroup
{
public:
    /// @brief Construct a new ExclusiveGroup.
    /// @tparam ...Args The types of the arguments to include in the group.
    /// @param ...args The arguments to include in the group.
    template <typename... Args>
        requires((!std::same_as<std::remove_cvref_t<Args>, ExclusiveGroup> && ...) &&
                 (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
    explicit ExclusiveGroup(Args &&...args)
        : ArgumentGroup(true, false, std::forward<Args>(args)...)
    {
    }
};

/// @brief Inclusive argument group.
class InclusiveGroup : public ArgumentGroup
{
public:
    /// @brief Construct a new InclusiveGroup.
    /// @tparam ...Args The types of the arguments to include in the group.
    /// @param ...args The arguments to include in the group.
    template <typename... Args>
        requires((!std::same_as<std::remove_cvref_t<Args>, InclusiveGroup> && ...) &&
                 (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
    explicit InclusiveGroup(Args &&...args)
        : ArgumentGroup(false, true, std::forward<Args>(args)...)
    {
    }
};
} // namespace cli::commands

#ifdef CHAIN_CLI_VERBOSE
#include <iostream>
#endif

namespace cli::logging
{

/// @brief Enumeration of log levels.
enum class LogLevel
{
    TRACE = 0, // most detailed, prints internal logs of the library as well
    VERBOSE,   // very detailed information for inspection/development
    DEBUG,     // general debug information
    INFO,      // general information
    SUCCESS,   // report a success
    WARNING,   // something unwanted happened, but the program can handle it
    ERROR,     // a serious error occurred/ something failed
};

inline std::string_view toString(LogLevel level)
{
    switch (level)
    {
        using enum cli::logging::LogLevel;
    case TRACE:
        return "TRACE";
    case VERBOSE:
        return "VERBOSE";
    case DEBUG:
        return "DEBUG";
    case SUCCESS:
        return "DETAIL";
    case INFO:
        return "INFO";
    case WARNING:
        return "WARNING";
    case ERROR:
        return "ERROR";
    }
    return "UNKNOWN";
}

} // namespace cli::logging

namespace cli::logging
{

/// @brief Data structure representing a log record.
struct LogRecord
{
    /// @brief The log level of the record.
    const LogLevel level;
    /// @brief The log message.
    const std::string message;
    /// @brief The timestamp of the log record.
    const std::chrono::system_clock::time_point timestamp{std::chrono::system_clock::now()};

    /// @brief Construct a new LogRecord.
    /// @param lvl The log level of the record.
    /// @param msg The log message.
    LogRecord(LogLevel lvl, std::string msg) : level(lvl), message(std::move(msg)) {}
};

} // namespace cli::logging

namespace cli::logging
{

/// @brief Interface for log record formatters.
class AbstractFormatter
{
public:
    virtual ~AbstractFormatter() = default;

    /// @brief Formats a log record into a string.
    /// @param record The log record to format.
    /// @return The formatted log record as a string.
    virtual std::string format(const LogRecord &record) const = 0;
};

/// @brief Basic implementation of a log record formatter that includes timestamp, log level, and
/// message.
class BasicFormatter : public AbstractFormatter
{
public:
    std::string format(const LogRecord &record) const override;

private:
    const std::chrono::time_zone *timezone = std::chrono::current_zone();
};

/// @brief Formatter that displays only the message of the log record.
class MessageOnlyFormatter : public AbstractFormatter
{
public:
    std::string format(const LogRecord &record) const override;
};

} // namespace cli::logging

#if defined(_MSC_VER)
constexpr const std::string ESC = "\x1B";
#else
constexpr const std::string ESC = "\033";
#endif

namespace cli::logging
{

// contains ANSI escape codes for styling log messages
using LogStyleMap = std::map<LogLevel, std::string>;

// Default console styles
inline LogStyleMap defaultStyles()
{
    using enum cli::logging::LogLevel;
    return {
        {TRACE, ESC + "[90m"},   // gray
        {VERBOSE, ESC + "[90m"}, // gray
        {DEBUG, ESC + "[36m"},   // cyan
        // Info uses plain grey text (no color)
        {SUCCESS, ESC + "[32m"}, // green
        {WARNING, ESC + "[33m"}, // yellow
        {ERROR, ESC + "[31m"},   // red
    };
}
} // namespace cli::logging

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

namespace cli
{
    /// @brief Thrown when an argument that was requested is missing in the context.
    class MissingArgumentException : public std::runtime_error
{
public:
    MissingArgumentException(const std::string &name,
                             const std::unordered_map<std::string, std::any> &args)
        : std::runtime_error(makeMessage(name, args))
    {
    }

private:
    static std::string makeMessage(const std::string &name,
                                   const std::unordered_map<std::string, std::any> &args);
};

/// @brief Thrown when an argument type that was requested is not the one that was parsed.
class InvalidArgumentTypeException : public std::runtime_error
{
public:
    InvalidArgumentTypeException(const std::string &name, const std::type_info &requested,
                                 const std::type_info &actual)
        : std::runtime_error(makeMessage(name, requested, actual))
    {
    }

private:
    static std::string makeMessage(const std::string &name, const std::type_info &requested,
                                   const std::type_info &actual);
};
} // namespace cli

namespace cli
{

/// @brief Represents the context of a command-line interface (CLI) invocation and as such contains
/// the parsed values (if present for all Arguments)
class CliContext
{
public:
    /// @brief Constructs a new CliContext object from the passed argument maps.
    /// @param posArgs a map of positional argument names to their values
    /// @param optArgs a map of optional argument names to their values
    /// @param flagArgs a set of flag argument names
    /// @param logger a logger instance to use in the methods this object is passed to
    explicit CliContext(std::unique_ptr<std::unordered_map<std::string, std::any>> posArgs,
                        std::unique_ptr<std::unordered_map<std::string, std::any>> optArgs,
                        std::unique_ptr<std::unordered_set<std::string>> flagArgs,
                        cli::logging::AbstractLogger &logger)
        : logger(logger), positionalArgs(std::move(posArgs)), optionArgs(std::move(optArgs)),
          flagArgs(std::move(flagArgs))
    {
    }

    // Non-copyable
    CliContext(const CliContext &) = delete;
    CliContext &operator=(const CliContext &) = delete;

    /// @brief Checks if an argument with the given name is present in the context.
    /// @param argName the name of the argument to check
    /// @return true if the argument is present, false otherwise
    bool isArgPresent(const std::string &argName) const;

    /// @brief Checks if an optional argument with the given name is present in the context.
    /// @param argName the name of the argument to check
    /// @return true if the argument is present, false otherwise
    bool isOptionArgPresent(const std::string &argName) const;

    /// @brief Checks if a positional argument with the given name is present in the context.
    /// @param argName the name of the argument to check
    /// @return true if the argument is present, false otherwise
    bool isPositionalArgPresent(const std::string &argName) const;

    /// @brief Checks if a flag with the given name is present in the context.
    /// @param argName the name of the argument to check
    /// @return true if the argument is present, false otherwise
    bool isFlagPresent(const std::string &argName) const;

    /// @brief Gets the value of a positional argument
    /// @tparam T the type to cast the argument value to (should be the same as the one used in
    /// defining the Argument)
    /// @param argName the name of the positional argument to retrieve
    /// @return the value of the positional argument cast to the specified type
    template <typename T> T getPositionalArg(const std::string &argName) const
    {
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Getting positional argument '" << argName << "' as type " << typeid(T).name() << "\n";
#endif
        return getAnyCast<T>(argName, *positionalArgs);
    }

    /// @brief Gets the value of a positional argument and stores it in the provided output variable
    /// @tparam T the type to cast the argument value to (should be the same as the one used in
    /// defining the Argument)
    /// @param argName the name of the positional argument to retrieve
    /// @param out the output variable to store the argument value in
    template <typename T> void getPositionalArg(const std::string &argName, T &out) const
    {
        out = getAnyCast<T>(argName, *positionalArgs);
    }

    /// @brief Gets the value of an optional argument
    /// @tparam T the type to cast the argument value to (should be the same as the one used in
    /// defining the Argument)
    /// @param argName the name of the optional argument to retrieve
    /// @return the value of the optional argument cast to the specified type
    template <typename T> T getOptionArg(const std::string &argName) const
    {
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Getting option argument '" << argName << "' as type " << typeid(T).name() << "\n";
#endif
        return getAnyCast<T>(argName, *optionArgs);
    }

    /// @brief Gets the value of an optional argument and stores it in the provided output variable
    /// @tparam T the type to cast the argument value to (should be the same as the one used in
    /// defining the Argument)
    /// @param argName the name of the optional argument to retrieve
    /// @param out the output variable to store the argument value in
    template <typename T> void getOptionArg(const std::string &argName, T &out) const
    {
        out = getAnyCast<T>(argName, *optionArgs);
    }

    /// @brief Gets all values of a repeatable option argument
    /// @tparam T the type to cast the single argument values to (should be the same as the one used
    /// in defining the Argument)
    /// @param argName the name of the repeatable option argument to retrieve
    /// @return a vector of all values of the repeatable option argument cast to the specified type
    template <typename T> std::vector<T> getRepeatableOptionArg(const std::string &argName) const
    {
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Getting repeatable option argument '" << argName << "' as vector of type " << typeid(T).name() << "\n";
#endif
        auto it = optionArgs->find(argName);
        if (it == optionArgs->end())
            throw MissingArgumentException(argName, *optionArgs);

        try
        {
            auto anyVec = getAnyCast<std::vector<std::any>>(argName, *optionArgs);
            std::vector<T> result;
            result.reserve(anyVec.size());
            for (const auto &elem : anyVec)
            {
                result.push_back(std::any_cast<T>(elem));
            }

            return result;
        }
        catch (const std::bad_any_cast &)
        {
            throw InvalidArgumentTypeException(argName, typeid(std::vector<T>), it->second.type());
        }
    }

    /// @brief Gets all values of a repeatable positional argument
    /// @tparam T the type to cast the single argument values to (should be the same as the one used
    /// in defining the Argument)
    /// @param argName the name of the repeatable positional argument to retrieve
    /// @return a vector of all values of the repeatable positional argument cast to the specified
    /// type
    template <typename T>
    std::vector<T> getRepeatablePositionalArg(const std::string &argName) const
    {
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Getting repeatable positional argument '" << argName << "' as vector of type " << typeid(T).name() << "\n";
#endif
        auto it = positionalArgs->find(argName);
        if (it == positionalArgs->end())
            throw MissingArgumentException(argName, *positionalArgs);

        try
        {
            auto anyVec = getAnyCast<std::vector<std::any>>(argName, *positionalArgs);
            std::vector<T> result;
            result.reserve(anyVec.size());
            for (const auto &elem : anyVec)
            {
                result.push_back(std::any_cast<T>(elem));
            }

            return result;
        }
        catch (const std::bad_any_cast &)
        {
            throw InvalidArgumentTypeException(argName, typeid(std::vector<T>), it->second.type());
        }
    }

    /// @brief Gets the value of an argument
    /// @tparam T the type to cast the argument value to (should be the same as the one used in
    /// defining the Argument)
    /// @param argName the name of the argument to retrieve
    /// @return the value of the argument cast to the specified type
    template <typename T> T getArg(const std::string &argName) const
    {
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Getting any argument '" << argName << "' as type " << typeid(T).name() << "\n";
#endif
        if (isPositionalArgPresent(argName))
        {
            return getAnyCast<T>(argName, *positionalArgs);
        }
        else if (isOptionArgPresent(argName))
        {
            return getAnyCast<T>(argName, *optionArgs);
        }
        else
        {
            throw MissingArgumentException(argName, *positionalArgs);
        }
    }

    /// @brief Gets all values of a repeatable argument
    /// @tparam T the type to cast the argument values to (should be the same as the one used in
    /// defining the Argument)
    /// @param argName the name of the repeatable argument to retrieve
    /// @return a vector of all values of the repeatable argument cast to the specified type
    template <typename T> auto getRepeatableArg(const std::string &argName) const
    {
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Getting repeatable argument '" << argName << "' as vector of type " << typeid(T).name() << "\n";
#endif
        if (isPositionalArgPresent(argName))
        {
            return getRepeatablePositionalArg<T>(argName);
        }
        else if (isOptionArgPresent(argName))
        {
            return getRepeatableOptionArg<T>(argName);
        }
        else
        {
            throw MissingArgumentException(argName, *positionalArgs);
        }
    }

    logging::AbstractLogger &Logger() const
    {
        return logger;
    }

private:
    cli::logging::AbstractLogger &logger;
    std::unique_ptr<std::unordered_map<std::string, std::any>> positionalArgs;
    std::unique_ptr<std::unordered_map<std::string, std::any>> optionArgs;
    std::unique_ptr<std::unordered_set<std::string>> flagArgs;

    template <typename T>
    static T getAnyCast(const std::string &name, std::unordered_map<std::string, std::any> &dict)
    {
        try
        {
            auto it = dict.find(name);
            if (it == dict.end())
            {
                throw MissingArgumentException(name, dict);
            }
            return std::any_cast<T>(it->second);
        }
        catch (const std::bad_any_cast &)
        {
            throw InvalidArgumentTypeException(name, typeid(T), dict.at(name).type());
        }
    }
};

} // namespace cli

namespace cli::parsing
{
/// @brief Exception thrown when parsing for an argument fails.
class ParseException : public std::runtime_error
{
public:
    /// @brief Construct a ParseException with a message, input string, and argument
    /// @param message The error message
    /// @param input The input string that failed to parse
    /// @param argument The argument that failed to parse
    ParseException(const std::string &message, const std::string &input, const cli::commands::ArgumentBase &argument)
        : std::runtime_error(message), input(input), argument(argument)
    {
    }

    /// @brief Construct a ParseException with default message, input string, and argument
    /// @param input The input string that failed to parse
    /// @param argument The argument that failed to parse
    ParseException(const std::string &input, const cli::commands::ArgumentBase &argument)
        : ParseException(std::format("Failed to parse input '{}' for argument: {}", input, argument.getName()), input, argument)
    {
    }

    /// @brief Gets the input string that failed to parse
    /// @return the input string
    const std::string &getInput() const noexcept { return input; }

    /// @brief Gets the argument that failed to parse
    /// @return reference to the argument
    const cli::commands::ArgumentBase &getArgument() const noexcept { return argument; }

private:
    std::string input;
    const cli::commands::ArgumentBase &argument;
};

/// @brief Exception thrown when the input string cannot be parsed to the needed type for an argument.
class TypeParseException : public std::runtime_error
{
public:
    /// @brief Construct a TypeParseException with a message, input string, and target type
    /// @param message The error message
    /// @param input The input string that couldn't be parsed
    /// @param targetType The type it couldn't be parsed to
    TypeParseException(const std::string &message, const std::string &input, const std::type_info &targetType)
        : std::runtime_error(message), input(input), targetType(targetType)
    {
    }

    /// @brief Construct a TypeParseException with default message, input string, and target type
    /// @param input The input string that couldn't be parsed
    /// @param targetType The type it couldn't be parsed to
    TypeParseException(const std::string &input, const std::type_info &targetType)
        : TypeParseException(std::format("Could not parse '{}' to type '{}'", input, targetType.name()), input, targetType)
    {
    }

    /// @brief Gets the input string that failed to parse
    /// @return the input string
    const std::string &getInput() const noexcept { return input; }

    /// @brief Gets the target type that the input couldn't be parsed to
    /// @return reference to the target type info
    const std::type_info &getTargetType() const noexcept { return targetType; }

private:
    std::string input;
    const std::type_info &targetType;
};

/// @brief Exception thrown when parsing a group of arguments fails.
class GroupParseException : public std::runtime_error
{
public:
    /// @brief Construct a GroupParseException with a message and argument group
    /// @param message The error message
    /// @param argumentGroup The argument group that failed to parse
    GroupParseException(const std::string &message, const cli::commands::ArgumentGroup &argumentGroup)
        : std::runtime_error(message), argumentGroup(argumentGroup)
    {
    }

    /// @brief Construct a GroupParseException with default message and argument group
    /// @param argumentGroup The argument group that failed to parse
    explicit GroupParseException(const cli::commands::ArgumentGroup &argumentGroup)
        : GroupParseException(std::format("Failed to parse argument group"), argumentGroup)
    {
    }

    /// @brief Gets the argument group that failed to parse
    /// @return reference to the argument group
    const cli::commands::ArgumentGroup &getArgumentGroup() const noexcept { return argumentGroup; }

private:
    const cli::commands::ArgumentGroup &argumentGroup;
};

} // namespace cli::parsing

namespace cli::parsing
{
/// @brief Helper struct providing static methods for parsing strings into various types.
struct ParseHelper
{
    /// @brief Parses a string input into a value of type T.
    /// @tparam T The type to parse the input into.
    /// @param input The input string to parse.
    /// @return The parsed value of type T.
    template <typename T> static T parse(const std::string &input)
    {
        std::istringstream iss(input);
        T value;

        if constexpr (std::is_same_v<T, std::string>)
        {
            return input; // For strings, just return
        }
        else
        {
            // Use operator>> for all other types
            iss >> value;
            if (iss.fail() || !iss.eof())
            {
                throw TypeParseException(std::format("Failed to parse value of type {} from input: {}", typeid(T).name(), input), input, typeid(T));
            }
        }

        return value;
    }

    /// @brief Parses a string input into a value of type T.
    /// @tparam T The type to parse the input into.
    /// @param input The input string to parse.
    /// @param value The variable to store the parsed value.
    template <typename T> static void parse(const std::string &input, T &value)
    {
        // Call the return-by-value version and assign
        value = parse<T>(input);
    }
};
} // namespace cli::parsing

namespace cli::commands
{

/// @brief Represents a flag argument in the CLI.
class FlagArgument : public ArgumentBase, public FlaggedArgumentBase
{
public:
    /// @brief Construct a new Flag Argument object.
    /// @param name The name of the argument (usually starts with "--").
    /// @param shortName The short name of the argument (usually starts with "-").
    /// @param optionsComment A comment describing this argument.
    /// @param isRequired Whether the argument is required.
    explicit FlagArgument(std::string_view name, std::string_view shortName = "",
                          std::string_view optionsComment = "", bool isRequired = false)
        : ArgumentBase(name, optionsComment, ArgumentKind::Flag, false, isRequired),
          FlaggedArgumentBase(shortName)
    {
    }

    [[nodiscard]] std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const override;
    [[nodiscard]] std::string getArgDocString(const docwriting::DocWriter &writer) const override;

#pragma region ChainingMethods
    /// @brief Set the options comment for the argument.
    /// @details The options comment is a brief description of the argument's purpose, used in
    /// help messages and documentation.
    /// @param comment The options comment to set.
    /// @return A reference to this argument.
    FlagArgument &withOptionsComment(std::string_view comment)
    {
        optionsComment = comment;
        return *this;
    }

    /// @brief Set whether the argument is required.
    /// @param req Whether the argument should be required.
    /// @return A reference to this argument.
    FlagArgument &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    /// @brief Set the short name for the argument.
    /// @details The short name is a single-character alias for the argument, usually prefixed with
    /// a single dash (e.g., "-h" for "--help").
    /// @param name The short name to set.
    /// @return A reference to this argument.
    FlagArgument &withShortName(std::string_view name)
    {
        this->shortName = name;
        return *this;
    }
#pragma endregion ChainingMethods
};

} // namespace cli::commands

namespace cli::commands
{

/// @brief Untemplated base class for option arguments in the CLI. Used to store all option
/// arguments in a single container.
class OptionArgumentBase : public TypedArgumentBase, public ArgumentBase, public FlaggedArgumentBase
{
public:
    /// @brief Construct a new Option Argument object.
    /// @param name The name of the argument (usually starts with "--").
    /// @param optionsComment A comment describing this argument.
    /// @param repeatable Whether the argument can be specified multiple times.
    /// @param required Whether the argument is required.
    /// @param t The type of the argument's value.
    /// @param shortName The short name for the argument (usually starts with "-").
    /// @param valueName The name of the value for the argument.
    OptionArgumentBase(std::string_view name, std::string_view optionsComment, bool repeatable,
                       bool required, std::type_index t, std::string_view shortName,
                       std::string_view valueName)
        : TypedArgumentBase(t),
          ArgumentBase(name, optionsComment, ArgumentKind::Option, repeatable, required),
          FlaggedArgumentBase(shortName), valueName(valueName)
    {
    }

    [[nodiscard]] std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const override;
    [[nodiscard]] std::string getArgDocString(const docwriting::DocWriter &writer) const override;
    [[nodiscard]] constexpr std::string_view getValueName() const noexcept { return valueName; }

protected:
    const std::string valueName;
};

/// @brief Represents option arguments in the CLI.
/// @tparam T The type of the argument's value.
template <typename T> class OptionArgument : public OptionArgumentBase
{

public:
    /// @brief Construct a new Option Argument object.
    /// @tparam T The type of the argument's value.
    /// @param name The name of the argument (usually starts with "--").
    /// @param valueName The name of the value for the argument.
    /// @param shortName The short name for the argument (usually starts with "-").
    /// @param optionsComment A comment describing this argument.
    /// @param required Whether the argument is required.
    /// @param repeatable Whether the argument can be specified multiple times.
    explicit OptionArgument(std::string_view name, std::string_view valueName,
                            std::string_view shortName = "", std::string_view optionsComment = "",
                            bool required = false, bool repeatable = false)
        : OptionArgumentBase(name, optionsComment, repeatable, required, typeid(T), shortName,
                             valueName)
    {
    }

    [[nodiscard]] std::any parseToValue(const std::string &input) const override;

#pragma region ChainingMethods

    /// @brief Set the options comment for the argument.
    /// @details The options comment is a brief description of the argument's purpose, used in
    /// help messages and documentation.
    /// @param comment The options comment to set.
    /// @return A reference to this argument.
    OptionArgument<T> &withOptionsComment(std::string_view comment)
    {
        this->optionsComment = comment;
        return *this;
    }

    /// @brief Set whether the argument is required.
    /// @param req Whether the argument should be required.
    /// @return A reference to this argument.
    OptionArgument<T> &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    /// @brief Set the short name for the argument.
    /// @details The short name is a single-character alias for the argument, usually prefixed with
    /// a single dash (e.g., "-h" for "--help").
    /// @param name The short name to set.
    /// @return A reference to this argument.
    OptionArgument<T> &withShortName(std::string_view name)
    {
        this->shortName = name;
        return *this;
    }

    /// @brief Set whether the argument can be specified multiple times.
    /// @param rep Whether the argument can be specified multiple times.
    /// @return A reference to this argument.
    OptionArgument<T> &withRepeatable(bool rep)
    {
        repeatable = rep;
        return *this;
    }
};

template <typename T>
inline std::any OptionArgument<T>::parseToValue(const std::string &input) const
{
    return cli::parsing::ParseHelper::parse<T>(input);
}
} // namespace cli::commands

namespace cli::commands
{
// untemplated base class for storing in same STL

/// @brief Untemplated Base class for positional arguments in the CLI. Used to store all positional
/// arguments in a single container.
class PositionalArgumentBase : public TypedArgumentBase, public ArgumentBase
{
public:
    /// @brief Construct a new Positional Argument object.
    /// @param name The name of the argument.
    /// @param optionsComment A comment describing this argument.
    /// @param repeatable Whether the argument can be specified multiple times.
    /// @param required Whether the argument is required.
    /// @param t The type of the argument's value.
    PositionalArgumentBase(std::string_view name, std::string_view optionsComment, bool repeatable,
                           bool required, std::type_index t)
        : TypedArgumentBase(t),
          ArgumentBase(name, optionsComment, ArgumentKind::Positional, repeatable, required)
    {
    }

    [[nodiscard]] std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const override;
    [[nodiscard]] std::string getArgDocString(const docwriting::DocWriter &writer) const override;
};

/// @brief Represents positional arguments in the CLI.
/// @tparam T The type of the argument's value.
template <typename T> class PositionalArgument : public PositionalArgumentBase
{

public:
    /// @brief Construct a new Positional Argument object.
    /// @tparam T The type of the argument's value.
    /// @param name The name of the argument.
    /// @param optionsComment A comment describing this argument.
    /// @param required Whether the argument is required.
    /// @param repeatable Whether the argument can be specified multiple times.
    explicit PositionalArgument(std::string_view name, std::string_view optionsComment = "",
                                bool required = true, bool repeatable = false)
        : PositionalArgumentBase(name, optionsComment, repeatable, required, typeid(T))
    {
    }

    [[nodiscard]] std::any parseToValue(const std::string &input) const override;

#pragma region ChainingMethods

    /// @brief Set the options comment for the argument.
    /// @details The options comment is a brief description of the argument's purpose, used in
    /// help messages and documentation.
    /// @param comment The options comment to set.
    /// @return A reference to this argument.
    PositionalArgument<T> &withOptionsComment(std::string_view comment)
    {
        optionsComment = comment;
        return *this;
    }

    /// @brief Set whether the argument is required.
    /// @param req Whether the argument should be required.
    /// @return A reference to this argument.
    PositionalArgument<T> &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    /// @brief Set whether the argument can be specified multiple times.
    /// @param rep Whether the argument can be specified multiple times.
    /// @return A reference to this argument.
    PositionalArgument<T> &withRepeatable(bool rep)
    {
        repeatable = rep;
        return *this;
    }

    PositionalArgument<T> &&create() { return std::move(*this); }

#pragma endregion ChainingMethods
};

template <typename T>
inline std::any PositionalArgument<T>::parseToValue(const std::string &input) const
{
    return cli::parsing::ParseHelper::parse<T>(input);
}

template <typename T> PositionalArgument<T> &createPositionalArgument(std::string_view id)
{
    static thread_local PositionalArgument<T> instance(id);
    return instance;
}

template <typename T>
PositionalArgument<T> &createPositionalArgument(std::string_view id,
                                                std::string_view optionsComment, bool required,
                                                bool repeatable)
{
    static thread_local PositionalArgument<T> instance(id, optionsComment, required, repeatable);
    return instance;
}

} // namespace cli::commands

namespace cli::commands
{

/// @brief Represents a command in the CLI application.
/// @details A command can have subcommands, arguments (positional, option, flag), and an execution
/// function. Commands can be nested to create a hierarchy of commands and subcommands.
class Command
{
    friend std::ostream &operator<<(std::ostream &out, const Command &cmd);

public:
    friend class cli::commands::docwriting::DocWriter;

    /// @brief Construct a new Command object.
    /// @param id The unique identifier for the command.
    /// @param short_desc A short description of the command.
    /// @param long_desc A long description of the command.
    /// @param actionPtr A pointer to the function to execute when the command is called.
    Command(std::string_view id, std::string_view short_desc, std::string_view long_desc,
            std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
        : identifier(id), shortDescription(short_desc), longDescription(long_desc),
          executePtr(std::move(actionPtr))
    {
    }

    /// @brief Construct a new Command object.
    /// @param id The unique identifier for the command.
    /// @param short_desc A short description of the command.
    /// @param long_desc A long description of the command.
    /// @param action A function to execute when the command is called.
    Command(std::string_view id, std::string_view short_desc, std::string_view long_desc,
            std::function<void(const CliContext &)> action)
        : identifier(id), shortDescription(short_desc), longDescription(long_desc),
          executePtr(std::make_unique<std::function<void(const CliContext &)>>(std::move(action)))
    {
    }

    /// @brief Construct a new Command object.
    /// @param id The unique identifier for the command.
    explicit Command(std::string_view id)
        : identifier(id), shortDescription(""), longDescription(""), executePtr(nullptr)
    {
    }

    // Movable
    Command(Command &&) noexcept = default;
    Command &operator=(Command &&) noexcept = default;

    // Non-copyable
    Command(const Command &) = delete;
    Command &operator=(const Command &) = delete;

    virtual ~Command() = default;

#pragma region Accessors

    /// @brief Get the unique identifier for the command.
    /// @return The unique identifier for the command.
    [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept { return identifier; }

    /// @brief Get the short description of the command.
    /// @return The short description of the command.
    [[nodiscard]] constexpr std::string_view getShortDescription() const noexcept
    {
        return shortDescription;
    }

    /// @brief Get the long description of the command.
    /// @return The long description of the command.
    [[nodiscard]] constexpr std::string_view getLongDescription() const noexcept
    {
        return longDescription;
    }

    /// @brief Check if the command has an execution function.
    /// @return True if the command has an execution function, false otherwise.
    [[nodiscard]] bool hasExecutionFunction() const noexcept { return executePtr.get(); }

    /// @brief Get the positional arguments for the command.
    /// @return The positional arguments for the command.
    [[nodiscard]] const std::vector<std::shared_ptr<PositionalArgumentBase>> &
    getPositionalArguments() const noexcept
    {
        return positionalArguments;
    }

    /// @brief Get the option arguments for the command.
    /// @note Arguments appear on the command line help messages in the order they were added to the
    /// command.
    /// @return The option arguments for the command.
    [[nodiscard]] const std::vector<std::shared_ptr<OptionArgumentBase>> &getOptionArguments()
        const noexcept
    {
        return optionArguments;
    }

    /// @brief Get the flag arguments for the command.
    /// @note Arguments appear on the command line help messages in the order they were added to the
    /// command.
    /// @return The flag arguments for the command.
    [[nodiscard]] const std::vector<std::shared_ptr<FlagArgument>> &getFlagArguments()
        const noexcept
    {
        return flagArguments;
    }

    /// @brief Get the argument groups for the command.
    /// @note Argument groups appear on the command line help messages in the order they were added
    /// @return The argument groups for the command.
    [[nodiscard]] const std::vector<std::unique_ptr<ArgumentGroup>> &getArgumentGroups()
        const noexcept
    {
        return argumentGroups;
    }

    /// @brief Get the short documentation string for the command.
    /// @details This description only contains the textual representation of the command and its
    /// arguments as well as the short description.
    /// @note the doc strings are cached internally and need to be built before being accessed
    /// @return The short documentation string for the command.
    [[nodiscard]] std::string_view getDocStringShort() const;

    /// @brief Get the long documentation string for the command.
    /// @details This description contains the textual representation of the command and its
    /// arguments as well as the long description and the Options segment.
    /// @note the doc strings are cached internally and need to be built before being accessed
    /// @return The long documentation string for the command.
    [[nodiscard]] std::string_view getDocStringLong() const;

    /// @brief Get a sub-command by its identifier.
    /// @param id The identifier of the sub-command.
    /// @return A pointer to the sub-command if found, nullptr otherwise.
    [[nodiscard]] Command *getSubCommand(std::string_view id);

    /// @brief Get a sub-command by its identifier.
    /// @param id The identifier of the sub-command.
    /// @return A pointer to the sub-command if found, nullptr otherwise.
    [[nodiscard]] const Command *getSubCommand(std::string_view id) const;

    /// @brief Get all sub-commands of the command.
    /// @return A reference to the map of identifiers to their sub-commands.
    [[nodiscard]] auto &getSubCommands() { return subCommands; }

    /// @brief Get all sub-commands of the command.
    /// @return A reference to the map of identifiers to their sub-commands.
    [[nodiscard]] auto const &getSubCommands() const { return subCommands; }

#pragma endregion Accessor

    /// @brief Execute the command.
    /// @param context The CLI context to use for execution.
    void execute(const CliContext &context) const;

#pragma region ChainingMethods
    /// @brief Set the short description for the command.
    /// @param desc The short description to set.
    /// @return A reference to this command.
    Command &withShortDescription(std::string_view desc);

    /// @brief Set the long description for the command.
    /// @param desc The long description to set.
    /// @return A reference to this command.
    Command &withLongDescription(std::string_view desc);

    /// @brief Add a positional argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to the
    /// command.
    /// @tparam T The type of the positional argument.
    /// @param arg The positional argument to set.
    /// @return A reference to this command.
    template <typename T>
    Command &withPositionalArgument(std::shared_ptr<PositionalArgument<T>> arg)
    {
        safeAddToArgGroup(arg);
        positionalArguments.push_back(arg);
        return *this;
    }

    /// @brief Add a positional argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to the
    /// command.
    /// @tparam T The type of the positional argument.
    /// @param arg The positional argument to set.
    /// @return A reference to this command.
    template <typename T> Command &withPositionalArgument(PositionalArgument<T> &&arg)
    {
        return withPositionalArgument(std::make_shared<PositionalArgument<T>>(std::move(arg)));
    }

    /// @brief Add a positional argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to the
    /// command.
    /// @tparam T The type of the positional argument.
    /// @param arg The positional argument to set.
    /// @return A reference to this command.
    template <typename T> Command &withPositionalArgument(PositionalArgument<T> &arg)
    {
        return withPositionalArgument(std::make_shared<PositionalArgument<T>>(arg));
    }

    /// @brief Add an option argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to
    /// @tparam T The type of the option argument.
    /// @param arg The option argument to set.
    /// @return A reference to this command.
    template <typename T> Command &withOptionArgument(std::shared_ptr<OptionArgument<T>> arg)
    {
        safeAddToArgGroup(arg);
        optionArguments.push_back(arg);
        return *this;
    }

    /// @brief Add an option argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to
    /// the command.
    /// @tparam T The type of the option argument.
    /// @param arg The option argument to set.
    /// @return A reference to this command.
    template <typename T> Command &withOptionArgument(OptionArgument<T> &&arg)
    {
        return withOptionArgument(std::make_shared<OptionArgument<T>>(std::move(arg)));
    }

    /// @brief Add an option argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to
    /// the command.
    /// @tparam T The type of the option argument.
    /// @param arg The option argument to set.
    /// @return A reference to this command.
    template <typename T> Command &withOptionArgument(OptionArgument<T> &arg)
    {
        return withOptionArgument(std::make_shared<OptionArgument<T>>(arg));
    }

    /// @brief Add a flag argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to
    /// the command.
    /// @param arg The flag argument to set.
    /// @return A reference to this command.
    Command &withFlagArgument(std::shared_ptr<FlagArgument> arg);

    /// @brief Add a flag argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to
    /// the command.
    /// @param arg The flag argument to set.
    /// @return A reference to this command.
    Command &withFlagArgument(FlagArgument &&arg);

    /// @brief Add a flag argument to the command.
    /// @note Arguments appear on the command line help messages in the order they were added to
    /// the command.
    /// @param arg The flag argument to set.
    /// @return A reference to this command.
    Command &withFlagArgument(FlagArgument &arg);

    /// @brief Set the execution function for the command.
    /// @param action The function to execute when the command is called.
    /// @return A reference to this command.
    Command &withExecutionFunc(std::unique_ptr<std::function<void(const CliContext &)>> actionPtr);

    /// @brief Set the execution function for the command.
    /// @param action The function to execute when the command is called.
    /// @return A reference to this command.
    Command &withExecutionFunc(std::function<void(const CliContext &)> &&action);

    /// @brief Add a sub-command to this command.
    /// @param subCommandPtr The sub-command to add.
    /// @return A reference to this command.
    Command &withSubCommand(std::unique_ptr<Command> subCommandPtr);

    Command &withSubCommand(Command &&subCommand);

    /// @brief Add a sub-command to this command.
    /// @tparam ...Args The types of the sub-commands.
    /// @param ...args The sub-commands to add.
    /// @return A reference to this command.
    template <typename... Args>
        requires((sizeof...(Args) > 0) &&
                 (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
    Command &withExclusiveGroup(Args &&...args);

    /// @brief Add an inclusive argument group to this command.
    /// @tparam ...Args The types of the sub-commands.
    /// @param ...args The sub-commands to add.
    /// @return A reference to this command.
    template <typename... Args>
        requires((sizeof...(Args) > 0) &&
                 (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
    Command &withInclusiveGroup(Args &&...args);
#pragma endregion ChainingMethods

private:
    size_t indexForNewArgGroup{0};
    void safeAddToArgGroup(const std::shared_ptr<ArgumentBase> &arg);
    void addArgGroup(const ArgumentGroup &argGroup);

    std::string identifier;
    std::string shortDescription;
    std::string longDescription;
    std::unique_ptr<std::function<void(const CliContext &)>> executePtr;

    // arguments
    std::vector<std::shared_ptr<PositionalArgumentBase>> positionalArguments;
    std::vector<std::shared_ptr<OptionArgumentBase>> optionArguments;
    std::vector<std::shared_ptr<FlagArgument>> flagArguments;
    std::vector<std::unique_ptr<ArgumentGroup>> argumentGroups;

    std::string docStringShort; // cached short doc string
    std::string docStringLong;  // cached long doc string

    std::map<std::string, std::unique_ptr<Command>, std::less<>> subCommands;
};

/// @brief Exception thrown when a command is malformed.
/// @details This can happen if the command is missing required arguments, has conflicting
/// arguments, or other issues that prevent it from being used correctly.
class MalformedCommandException : public std::runtime_error
{
public:
    explicit MalformedCommandException(const Command &cmd, const std::string &msg = "")
        : std::runtime_error(buildMessage(cmd, msg)), malformedCmd(&cmd)
    {
    }

    const Command &command() const noexcept { return *malformedCmd; }

private:
    const Command *malformedCmd;

    static std::string buildMessage(const Command &cmd, const std::string &msg);
};

template <typename... Args>
    requires((sizeof...(Args) > 0) &&
             (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
inline Command &Command::withExclusiveGroup(Args &&...args)
{
    argumentGroups.emplace_back(std::make_unique<ExclusiveGroup>(std::forward<Args>(args)...));
    addArgGroup(*argumentGroups.back());
    indexForNewArgGroup++;
    return *this;
}

template <typename... Args>
    requires((sizeof...(Args) > 0) &&
             (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
inline Command &Command::withInclusiveGroup(Args &&...args)
{
    argumentGroups.emplace_back(std::make_unique<InclusiveGroup>(std::forward<Args>(args)...));
    addArgGroup(*argumentGroups.back());
    indexForNewArgGroup++;
    return *this;
}

} // namespace cli::commands

namespace cli::commands
{

/// @brief Exception thrown when a command is not found in the command tree.
class CommandNotFoundException : public std::runtime_error
{
public:
    /// @brief Construct a new CommandNotFoundException.
    /// @param missingId The ID of the missing command.
    /// @param chain The chain of command IDs leading to the missing command.
    CommandNotFoundException(const std::string &missingId, const std::vector<std::string> &chain)
        : std::runtime_error(buildMessage(missingId, chain)), missing(missingId), path(chain)
    {
    }

    const std::string &missingId() const noexcept { return missing; }

    const std::vector<std::string> &insertChain() const noexcept { return path; }

private:
    std::string missing;
    std::vector<std::string> path;

    static std::string buildMessage(const std::string &id, const std::vector<std::string> &chain);
};

/// @brief Tree structure to manage commands and their subcommands.
/// @details Each node in the tree is a Command, and commands can have multiple subcommands.
/// The tree allows for insertion, lookup, and traversal of commands.
class CommandTree
{
public:
    /// @brief Construct a new CommandTree.
    /// @param rootName The name of the root command.
    explicit CommandTree(std::string_view rootName);

    /// @brief Insert a command into the tree.
    /// @tparam ...Ids The types of the parent command IDs.
    /// @param cmd The command to insert.
    /// @param parentId The ID of the direct parent command (used to split first value of ...rest).
    /// @param ...rest The IDs of any additional parent commands.
    template <typename... Ids>
    void insert(std::unique_ptr<Command> cmd, std::string_view parentId, Ids &&...rest)
    {
        Command *parentCommandPtr = find(parentId, std::forward<Ids>(rest)...);
        if (!parentCommandPtr)
        {
            std::vector<std::string> chain{std::string(parentId), std::string(rest)...};
            throw CommandNotFoundException(std::string(parentId), chain);
        }

        parentCommandPtr->withSubCommand(std::move(cmd));
    }

    /// @brief Insert a command into the tree.
    /// @param cmd The command to insert.
    void insert(std::unique_ptr<Command> cmd) // insert at root
    {
        root->withSubCommand(std::move(cmd));
    }

    /// @brief Find a command in the tree by a path of identifiers leading to it.
    /// @tparam ...Ids The types of the command IDs.
    /// @param ...ids The command IDs.
    /// @return A pointer to the found command, or nullptr if not found.
    template <typename... Ids> Command *find(Ids &&...ids) const
    {
        return findRecursive(root.get(), std::forward<Ids>(ids)...);
    }

    /// @brief Apply a function to each command in the tree.
    /// @details The function is applied in a depth-first search (DFS) manner.
    /// @param func The function to apply.
    void forEachCommand(const std::function<void(Command *)> &func) const
    {
        if (root)
        {
            forEachCommandRecursive(root.get(), func);
        }
    }

    /// @brief Apply a function to each command in the tree.
    /// @details The function is applied in a depth-first search (DFS) manner.
    /// @param func The function to apply.
    void forEachCommand(const std::function<void(Command &)> &func) const
    {
        if (root)
        {
            forEachCommandRecursive(root.get(), func);
        }
    }

    /// @brief Get the root command of the tree.
    /// @return A pointer to the root command.
    Command *getRootCommand() { return root.get(); }

    /// @brief Get the root command of the tree.
    /// @return A pointer to the root command.
    const Command *getRootCommand() const { return root.get(); }

    /// @brief Get the path for a command in the tree.
    /// @note Uses a pre-built map for O(1) lookup internally that maps needs to be constructed
    /// first using the buildCommandPathMap function.
    /// @param cmd The command to find the path for.
    /// @return The path to the command, or an empty string if not found.
    std::string_view getPathForCommand(Command *cmd) const;

    /// @brief Build a map of command paths for quick lookup.
    /// @param separator The separator to use between command names in the path (default is a
    /// space).
    void buildCommandPathMap(const std::string &separator = " ");

    /// @brief Get a vector of all commands in the tree.
    /// @details Commands are collected in a depth-first search (DFS) manner.
    /// @return A vector containing pointers to all commands in the tree.
    std::vector<Command *> getAllCommands() const;

    /// @brief Get a vector of all commands in the tree (const version).
    /// @details Commands are collected in a depth-first search (DFS) manner.
    /// @return A vector containing pointers to all commands in the tree.
    std::vector<const Command *> getAllCommandsConst() const;

private:
    std::unique_ptr<Command> root;
    std::unordered_map<Command *, std::string> commandPathMap;

    void buildCommandPathMapRecursive(Command *cmd, std::vector<std::string> &path,
                                      const std::string &separator);

    // Recursive finder
    template <typename... Ids>
    static Command *findRecursive(Command *cmdPtr, std::string_view id, Ids &&...rest)
    {
        auto it = cmdPtr->getSubCommands().find(std::string(id));
        if (it == cmdPtr->getSubCommands().end())
            return nullptr;

        Command *subCommandPtr = it->second.get();

        if constexpr (sizeof...(rest) == 0)
        {
            // Base case: no more ids, return the child
            return subCommandPtr;
        }
        // Recursive case: continue with remaining ids
        return findRecursive(subCommandPtr, std::forward<Ids>(rest)...);
    }

    static Command *findRecursive(Command *cmdPtr) { return cmdPtr; }

    // Recursive DFS helper
    static void forEachCommandRecursive(Command *cmdPtr, const std::function<void(Command *)> &func)
    {
        if (cmdPtr)
        {
            func(cmdPtr); // call user-provided function
            for (const auto &[key, subCommandPtr] : cmdPtr->getSubCommands())
            {
                forEachCommandRecursive(subCommandPtr.get(), func);
            }
        }
    }

    static void forEachCommandRecursive(Command *cmdPtr, const std::function<void(Command &)> &func)
    {
        if (cmdPtr)
        {
            func(*cmdPtr); // call user-provided function
            for (const auto &[key, subCommandPtr] : cmdPtr->getSubCommands())
            {
                forEachCommandRecursive(subCommandPtr.get(), func);
            }
        }
    }

    // Helper function to collect all commands into a vector
    static void getAllCommandsRecursive(Command *cmdPtr, std::vector<Command *> &commands)
    {
        if (cmdPtr)
        {
            commands.push_back(cmdPtr);
            for (const auto &[key, subCommandPtr] : cmdPtr->getSubCommands())
            {
                getAllCommandsRecursive(subCommandPtr.get(), commands);
            }
        }
    }

    // Helper function to collect all commands into a vector (const version)
    static void getAllCommandsRecursive(const Command *cmdPtr, std::vector<const Command *> &commands)
    {
        if (cmdPtr)
        {
            commands.push_back(cmdPtr);
            for (const auto &[key, subCommandPtr] : cmdPtr->getSubCommands())
            {
                getAllCommandsRecursive(subCommandPtr.get(), commands);
            }
        }
    }
};
} // namespace cli::commands

namespace cli::commands::docwriting
{

/// @brief Abstract base class for argument documentation formatters.
/// @tparam T The type of argument to format.
template <typename T> class AbstractArgDocFormatter
{
public:
    virtual ~AbstractArgDocFormatter() = default;

    /// @brief Generate the argument documentation string.
    /// @details the argument doc string is the textual representation of the argument and its
    /// attributes
    /// @param argument The argument to document.
    /// @param configuration The CLI configuration.
    /// @return The generated documentation string.
    virtual std::string generateArgDocString(const T &argument,
                                             const cli::CliConfig &configuration) = 0;

    /// @brief Generate the options documentation string.
    /// @details the options doc string is used in the options section of the help message
    /// @param argument The argument to document.
    /// @param configuration The CLI configuration.
    /// @return The generated documentation string.
    virtual std::string generateOptionsDocString(const T &argument,
                                                 const cli::CliConfig &configuration) = 0;
};

/// @brief Default formatter for flag arguments.
class DefaultFlagFormatter : public AbstractArgDocFormatter<FlagArgument>
{
public:
    std::string generateArgDocString(const FlagArgument &argument,
                                     const cli::CliConfig &configuration) override;
    std::string generateOptionsDocString(const FlagArgument &argument,
                                         const cli::CliConfig &configuration) override;
};

/// @brief Default formatter for option arguments.
class DefaultOptionFormatter : public AbstractArgDocFormatter<OptionArgumentBase>
{
public:
    std::string generateArgDocString(const OptionArgumentBase &argument,
                                     const cli::CliConfig &configuration) override;
    std::string generateOptionsDocString(const OptionArgumentBase &argument,
                                         const cli::CliConfig &configuration) override;
};

/// @brief Default formatter for positional arguments.
class DefaultPositionalFormatter : public AbstractArgDocFormatter<PositionalArgumentBase>
{
public:
    std::string generateArgDocString(const PositionalArgumentBase &argument,
                                     const cli::CliConfig &configuration) override;
    std::string generateOptionsDocString(const PositionalArgumentBase &argument,
                                         const cli::CliConfig &configuration) override;
};

/// @brief Abstract base class for command documentation formatters.
class AbstractCommandFormatter
{
public:
    virtual ~AbstractCommandFormatter() = default;

    /// @brief Generate the command documentation string.
    /// @param command The command to document.
    /// @param fullCommandPath The full path of the command.
    /// @param writer The documentation writer.
    /// @param configuration The CLI configuration.
    /// @return The generated documentation string.
    virtual std::string generateLongDocString(const Command &command,
                                              std::string_view fullCommandPath,
                                              const DocWriter &writer,
                                              const cli::CliConfig &configuration) = 0;

    /// @brief Generate the short documentation string.
    /// @param command The command to document.
    /// @param fullCommandPath The full path of the command.
    /// @param writer The documentation writer.
    /// @param configuration The CLI configuration.
    /// @return The generated documentation string.
    virtual std::string generateShortDocString(const Command &command,
                                               std::string_view fullCommandPath,
                                               const DocWriter &writer,
                                               const cli::CliConfig &configuration) = 0;
};

/// @brief Default formatter for commands.
class DefaultCommandFormatter : public AbstractCommandFormatter
{
public:
    std::string generateLongDocString(const Command &command, std::string_view fullCommandPath,
                                      const DocWriter &writer,
                                      const cli::CliConfig &configuration) override;

    std::string generateShortDocString(const Command &command, std::string_view fullCommandPath,
                                       const DocWriter &writer,
                                       const cli::CliConfig &configuration) override;
};

/// @brief Abstract base class for CLI application documentation formatters.
class AbstractCliAppDocFormatter
{
public:
    virtual ~AbstractCliAppDocFormatter() = default;

    /// @brief Generate the application documentation string that shows all the available commands.
    /// @param writer The documentation writer.
    /// @param configuration The CLI configuration.
    /// @param commands The list of commands to document.
    /// @return The generated documentation string.
    virtual std::string generateAppDocString(
        const cli::CliConfig &configuration,
        const std::vector<const cli::commands::Command *> &commands) = 0;

    /// @brief Generate the application version string that is shown with the --version flag.
    /// @param configuration The CLI configuration.
    /// @return The generated version string.
    virtual std::string generateAppVersionString(const cli::CliConfig &configuration) = 0;

    /// @brief Generate the documentation string for a specific command.
    /// @param command The command to document.
    /// @param fullCommandPath The full path of the command.
    /// @param writer The documentation writer.
    /// @param configuration The CLI configuration.
    /// @return The generated documentation string.
    virtual std::string generateCommandDocString(const Command &command,
                                                 const cli::CliConfig &configuration) = 0;
};

/// @brief Default formatter for CLI application documentation.
class DefaultCliAppDocFormatter : public AbstractCliAppDocFormatter
{
public:
    std::string generateAppDocString(
        const cli::CliConfig &configuration,
        const std::vector<const cli::commands::Command *> &commands) override;

    std::string generateCommandDocString(
        const Command &command,
        [[maybe_unused]] const cli::CliConfig &configuration) override;

    std::string generateAppVersionString(const cli::CliConfig &configuration) override;
};

} // namespace cli::commands::docwriting

namespace cli::commands::docwriting
{

/// @brief Documentation writer for CLI commands. Consists of formatters for commands and arguments.
class DocWriter
{

public:
    /// @brief Construct a new DocWriter object.
    /// @param config The CLI configuration.
    explicit DocWriter(const CliConfig &config) : configuration(config) {}

    /// @brief Set the formatter for the application documentation.
    /// @param formatter The formatter to use for the application documentation.
    void setAppFormatter(std::unique_ptr<AbstractCliAppDocFormatter> formatter);

    /// @brief Set the formatter for option arguments.
    /// @param formatter The formatter to use for option arguments.
    void setOptionFormatter(std::unique_ptr<AbstractArgDocFormatter<OptionArgumentBase>> formatter);

    /// @brief Set the formatter for positional arguments.
    /// @param formatter The formatter to use for positional arguments.
    void setPositionalFormatter(
        std::unique_ptr<AbstractArgDocFormatter<PositionalArgumentBase>> formatter);

    /// @brief Set the formatter for flag arguments.
    /// @param formatter The formatter to use for flag arguments.
    void setFlagFormatter(std::unique_ptr<AbstractArgDocFormatter<FlagArgument>> formatter);

    /// @brief Set the formatter for command arguments.
    /// @param formatter The formatter to use for command arguments.
    void setCommandFormatter(std::unique_ptr<AbstractCommandFormatter> formatter);

    /// @brief Build and set the documentation strings for a command.
    /// @param command The command to set the documentation strings for.
    /// @param fullCommandPath The full path of the command.
    void setDocStrings(Command &command, std::string_view fullCommandPath) const;

    /// @brief Generate the long documentation string for a command.
    /// @param command The command to generate the documentation string for.
    /// @param fullCommandPath The full path of the command.
    /// @return The long documentation string for the command.
    std::string generateLongDocString(const Command &command,
                                      std::string_view fullCommandPath) const;

    /// @brief Generate the short documentation string for a command.
    /// @param command The command to generate the documentation string for.
    /// @param fullCommandPath The full path of the command.
    /// @return The short documentation string for the command.
    std::string generateShortDocString(const Command &command,
                                       std::string_view fullCommandPath) const;

    /// @brief Generate the documentation string for a flag argument.
    /// @param argument The flag argument to generate the documentation string for.
    /// @return The documentation string for the flag argument.
    std::string generateOptionsDocString(const FlagArgument &argument) const;

    /// @brief Generate the argument documentation string for a flag argument.
    /// @param argument The flag argument to generate the documentation string for.
    /// @return The argument documentation string for the flag argument.
    std::string generateArgDocString(const FlagArgument &argument) const;

    /// @brief Generate the documentation string for an option argument.
    /// @param argument The option argument to generate the documentation string for.
    /// @return The documentation string for the option argument.
    std::string generateOptionsDocString(const OptionArgumentBase &argument) const;

    /// @brief Generate the argument documentation string for an option argument.
    /// @param argument The option argument to generate the documentation string for.
    /// @return The argument documentation string for the option argument.
    std::string generateArgDocString(const OptionArgumentBase &argument) const;

    /// @brief Generate the documentation string for a positional argument.
    /// @param argument The positional argument to generate the documentation string for.
    /// @return The documentation string for the positional argument.
    std::string generateOptionsDocString(const PositionalArgumentBase &argument) const;

    /// @brief Generate the argument documentation string for a positional argument.
    /// @param argument The positional argument to generate the documentation string for.
    /// @return The argument documentation string for the positional argument.
    std::string generateArgDocString(const PositionalArgumentBase &argument) const;

    /// @brief Generate the documentation string for the application.
    /// @param commands The commands to generate the documentation string for.
    /// @return The documentation string for the application.
    std::string generateAppDocString(const std::vector<const cli::commands::Command *> &commands) const;

    /// @brief Generate the documentation string for a command.
    /// @param command The command to generate the documentation string for.
    /// @param fullCommandPath The full path of the command.
    /// @return The documentation string for the command.
    std::string generateCommandDocString(const Command &command) const;

    /// @brief Generate the version string for the application.
    /// @return The version string for the application.
    std::string generateAppVersionString() const;

private:
    const CliConfig &configuration;

    std::unique_ptr<AbstractCommandFormatter> commandFormatterPtr =
        std::make_unique<DefaultCommandFormatter>();
    std::unique_ptr<AbstractArgDocFormatter<FlagArgument>> flagFormatterPtr =
        std::make_unique<DefaultFlagFormatter>();
    std::unique_ptr<AbstractArgDocFormatter<OptionArgumentBase>> optionFormatterPtr =
        std::make_unique<DefaultOptionFormatter>();
    std::unique_ptr<AbstractArgDocFormatter<PositionalArgumentBase>> positionalFormatterPtr =
        std::make_unique<DefaultPositionalFormatter>();
    std::unique_ptr<AbstractCliAppDocFormatter> appFormatterPtr = std::make_unique<DefaultCliAppDocFormatter>();
};

} // namespace cli::commands::docwriting

namespace cli
{

/// @brief Builder for CliContext objects, allowing to incrementally add arguments before
/// constructing the final context object.
class ContextBuilder
{
public:
    /// @brief Constructs a new ContextBuilder instance.
    ContextBuilder();

    /// @brief Add a positional argument to the context being built.
    /// @param argName the name of the positional argument
    /// @param val value of the positional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addPositionalArgument(const std::string &argName, std::any &val);

    /// @brief Add a positional argument to the context being built.
    /// @param argName the name of the positional argument
    /// @param val value of the positional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addPositionalArgument(std::string_view argName, std::any &val);

    /// @brief Add a repeatable positional argument to the context being built.
    /// @param argName the name of the repeatable positional argument
    /// @param vals values of the repeatable positional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addRepeatablePositionalArgument(const std::string &argName, const std::vector<std::any> &vals);

    /// @brief Add a repeatable positional argument to the context being built.
    /// @param argName the name of the repeatable positional argument
    /// @param vals values of the repeatable positional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addRepeatablePositionalArgument(std::string_view argName, const std::vector<std::any> &vals);

    /// @brief Add an optional argument to the context being built.
    /// @param argName the name of the optional argument
    /// @param val  value of the optional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addOptionArgument(const std::string &argName, std::any &val);

    /// @brief Add an optional argument to the context being built.
    /// @param argName the name of the optional argument
    /// @param val  value of the optional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addOptionArgument(std::string_view argName, std::any &val);

    /// @brief Add a repeatable optional argument to the context being built.
    /// @param argName the name of the repeatable optional argument
    /// @param vals values of the repeatable optional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addRepeatableOptionArgument(const std::string &argName, const std::vector<std::any> &vals);

    /// @brief Add a repeatable optional argument to the context being built.
    /// @param argName the name of the repeatable optional argument
    /// @param vals values of the repeatable optional argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addRepeatableOptionArgument(std::string_view argName, const std::vector<std::any> &vals);

    /// @brief Add a flag argument to the context being built.
    /// @param argName the name of the flag argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addFlagArgument(const std::string &argName);

    /// @brief Add a flag argument to the context being built
    /// @param argName the name of the flag argument
    /// @return a reference to this ContextBuilder instance
    ContextBuilder &addFlagArgument(std::string_view argName);

    /// @brief Checks if an argument with the given name is present in the context being built.
    /// @param argName the name of the argument to search for
    /// @return true if the argument is present, false otherwise
    bool isArgPresent(const std::string &argName) const;

    /// @brief Builds the final CliContext object from the accumulated arguments.
    /// @param logger the logger instance to use in the created context
    /// @return a unique_ptr to the created CliContext object
    std::unique_ptr<CliContext> build(cli::logging::AbstractLogger &logger);

private:
    std::unique_ptr<std::unordered_map<std::string, std::any>> positionalArgs;
    std::unique_ptr<std::unordered_map<std::string, std::any>> optionalArgs;
    std::unique_ptr<std::unordered_set<std::string>> flagArgs;
};

} // namespace cli

namespace cli::parsing
{
/// @brief Class used to parse string inputs into typed argument values based on the command
/// definition.
class Parser
{
public:
    /// @brief Creates a new Parser instance with the given configuration.
    /// @param config The configuration to use for the parser.
    explicit Parser(const CliConfig &config) : configuration(config) {}

    /// @brief Parse the given inputs according to the specified command and populate the context
    /// builder with the parsed values.
    /// @param command The command to parse the inputs for.
    /// @param inputs The inputs to parse.
    /// @param contextBuilder The context builder to populate with the parsed values.
    void parseArguments(const cli::commands::Command &command,
                        const std::vector<std::string> &inputs,
                        ContextBuilder &contextBuilder) const;

private:
    std::vector<std::any> parseRepeatableList(const cli::commands::TypedArgumentBase &arg,
                                 const std::string &input) const;

    void parseRepeatable(const cli::commands::OptionArgumentBase &arg, const std::string &input,
                         ContextBuilder &ContextBuilder) const;

    void parseRepeatable(const cli::commands::PositionalArgumentBase &arg, const std::string &input,
                         ContextBuilder &ContextBuilder) const;

    void checkGroupsAndRequired(const cli::commands::Command &command,
                     const ContextBuilder &contextBuilder) const;

    bool tryOptionArg(
        const std::vector<std::shared_ptr<cli::commands::OptionArgumentBase>> &optionArguments,
        const std::vector<std::string> &inputs, const std::string &currentParsing, size_t index,
        ContextBuilder &contextBuilder) const;

    bool tryFlagArg(const std::vector<std::shared_ptr<cli::commands::FlagArgument>> &flagArguments,
                    const std::string &currentParsing, ContextBuilder &contextBuilder) const;

    const CliConfig &configuration;
};
} // namespace cli::parsing

#define RUN_CLI_APP(cliInstance, argc_, argv_)                                                     \
    try                                                                                            \
    {                                                                                              \
        return cliInstance.run(argc_, argv_);                                                      \
    }                                                                                              \
    catch (const std::exception &e)                                                                \
    {                                                                                              \
        cliInstance.Logger().error() << e.what() << std::endl;                                     \
        std::abort();                                                                              \
    }

namespace cli
{
/// @brief Main class representing a command-line application
class CliApp
{
public:
    // Non-copyable
    CliApp(const CliApp &) = delete;
    CliApp &operator=(const CliApp &) = delete;

    explicit CliApp(std::string_view executableName);
    explicit CliApp(CliConfig &&config);
    explicit CliApp(const CliConfig &config, std::unique_ptr<logging::AbstractLogger> logger);
    ~CliApp() = default;

    /// @brief Add a new command to the application
    /// @param subCommandPtr the unique pointer to the command to add
    /// @return a reference to this CliApp instance
    CliApp &withCommand(std::unique_ptr<commands::Command> subCommandPtr);

    /// @brief Add a new command to the application
    /// @param subCommand the command to add
    /// @return a reference to this CliApp instance
    CliApp &withCommand(commands::Command &&subCommand);

    /// @brief Initialize the CLI application, preparing it for execution
    /// This method sets up internal structures and should be called before `run()`
    /// if any commands have been added. If not called explicitly, it will be called
    /// automatically on the first invocation of `run()`.
    void init();

    /// @brief Run the CLI application with the given arguments
    /// @param argc the argument count
    /// @param argv the argument vector
    /// @return the exit code of the application
    int run(int argc, char *argv[]);

    /// @brief Get the logger instance used by the CLI application
    /// @return a reference to the logger instance
    [[nodiscard]] logging::AbstractLogger &Logger() { return *logger; }

    /// @brief Get the command tree used by the CLI application
    /// @return a reference to the command tree
    [[nodiscard]] const commands::CommandTree &getCommandTree() const { return commandsTree; };

    /// @brief Get the command tree used by the CLI application
    /// @return a reference to the command tree
    [[nodiscard]] commands::Command *getMainCommand() { return commandsTree.getRootCommand(); };

    /// @brief Get the configuration used by the CLI application
    /// @return a reference to the configuration
    [[nodiscard]] CliConfig &getConfig() { return *configuration; };

    /// @brief Get the documentation writer used by the CLI application
    /// @return a reference to the documentation writer
    [[nodiscard]] commands::docwriting::DocWriter &getDocWriter() { return docWriter; }

    /// @brief Set the logger instance used by the CLI application
    /// @param newLogger the new logger instance
    void setLogger(std::unique_ptr<logging::Logger> &&newLogger) { logger = std::move(newLogger); }

private:
    int internalRun(std::span<char *const> args);
    bool rootShortCircuits(std::vector<std::string> &args, const cli::commands::Command &cmd) const;
    bool commandShortCircuits(std::vector<std::string> &args, const cli::commands::Command *cmd) const;
    bool initialized{false};
    commands::CommandTree commandsTree;

    std::unique_ptr<CliConfig> configuration;
    std::unique_ptr<logging::AbstractLogger> logger;

    parsing::Parser parser;
    cli::commands::docwriting::DocWriter docWriter;
};
} // namespace cli

namespace cli
{
inline CliApp::CliApp(CliConfig &&config)
    : commandsTree(config.executableName),
      configuration(std::make_unique<CliConfig>(std::move(config))),
      logger(std::make_unique<logging::Logger>()),
      parser(*configuration),
      docWriter(*configuration)
{
}

inline CliApp::CliApp(std::string_view executableName)
    : commandsTree(executableName),
      configuration(std::make_unique<CliConfig>()),
      logger(std::make_unique<logging::Logger>()),
      parser(*configuration),
      docWriter(*configuration)
{
    configuration->executableName = std::string(executableName);
}

inline CliApp::CliApp(const CliConfig &config, std::unique_ptr<logging::AbstractLogger> logger)
    : commandsTree(config.executableName),
      configuration(std::make_unique<CliConfig>(config)),
      logger(std::move(logger)),
      parser(*configuration),
      docWriter(*configuration)
{
}

inline CliApp &CliApp::withCommand(std::unique_ptr<commands::Command> subCommandPtr)
{
    commandsTree.insert(std::move(subCommandPtr));
    return *this;
}

inline void CliApp::init()
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Initializing CLI application...\n";
#endif
    initialized = true;

    commandsTree.buildCommandPathMap();

#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Building documentation strings for commands...\n";
#endif
    commandsTree.forEachCommand([this](commands::Command *cmd) {
        docWriter.setDocStrings(*cmd, commandsTree.getPathForCommand(cmd));
    });
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
inline int CliApp::run(int argc, char *argv[])
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Starting CLI application with " << argc << " arguments\n";
    std::cout << "Arguments: ";
    for (int i = 0; i < argc; ++i) {
        std::cout << "\"" << argv[i] << "\" ";
    }
    std::cout << "\n";
#endif
    if (!initialized)
    {
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Application not initialized, initializing now...\n";
#endif
        init();
    }
    return internalRun(std::span<char *const>(argv + 1, argc - 1));
}

// returns the found command and modifies args to only contain the values that
// werent consumed in the tree traversal
inline commands::Command *locateCommand(commands::CommandTree &commandsTree, std::vector<std::string> &args)
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Locating command in tree with arguments: ";
    for (const auto &arg : args) {
        std::cout << "\"" << arg << "\" ";
    }
    std::cout << "\n";
#endif
    commands::Command *commandPtr = commandsTree.getRootCommand();

    int consumed = 0;

    for (const auto &arg : args)
    {
        // Move one level down if child exists
        commands::Command *subCommandPtr = commandPtr->getSubCommand(arg);
        if (!subCommandPtr)
        {
            break;
        }
        
        commandPtr = subCommandPtr;
        ++consumed;
    }
    args.erase(args.begin(), args.begin() + consumed);
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Located command: " << commandPtr->getIdentifier() << ", consumed " << consumed << " arguments\n";
    std::cout << "Remaining arguments: ";
    for (const auto &arg : args) {
        std::cout << "\"" << arg << "\" ";
    }
    std::cout << "\n";
#endif
    return commandPtr;
}

inline int CliApp::internalRun(std::span<char *const> args)
{
    std::vector<std::string> argVec(args.begin(), args.end());

    if (rootShortCircuits(argVec, *(commandsTree.getRootCommand())))
    {
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Root short-circuit triggered, exiting early\n";
#endif
        return 0;
    }

    if (const commands::Command *cmd = locateCommand(commandsTree, argVec);
        cmd && cmd->hasExecutionFunction())
    {
        if (commandShortCircuits(argVec, cmd))
        {
#ifdef CHAIN_CLI_VERBOSE
            std::cout << "Command short-circuit triggered for: " << cmd->getIdentifier() << "\n";
#endif
            return 0;
        }

        #ifdef CHAIN_CLI_VERBOSE
        std::cout << "Executing command: " << cmd->getIdentifier() << "\n";
        #endif

        auto contextBuilder = cli::ContextBuilder();

#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Parsing arguments for command execution...\n";
#endif
        parser.parseArguments(*cmd, argVec, contextBuilder);
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Building context and executing command...\n";
#endif
        cmd->execute(*contextBuilder.build(*logger));
    }
    else
    {
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "No valid command found or command has no execution function\n";
#endif
        logger->error() << "Unknown command: " << args[0] << "\n" << std::flush;
        auto allCommands = commandsTree.getAllCommandsConst();
        logger->info(docWriter.generateAppDocString(allCommands));
    }
    return 0;
}

inline bool CliApp::rootShortCircuits(std::vector<std::string> &args,
                               const cli::commands::Command &cmd) const
{
    if (args.empty() && !cmd.hasExecutionFunction())
    {
        auto allCommands = commandsTree.getAllCommandsConst();
        logger->info(docWriter.generateAppDocString(allCommands));
        return true;
    }

    if (args.size() == 1)
    {
        if (args.at(0) == "-h" || args.at(0) == "--help")
        {
            auto allCommands = commandsTree.getAllCommandsConst();
            logger->info(docWriter.generateAppDocString(allCommands));
            return true;
        }
        else if (args.at(0) == "-v" || args.at(0) == "--version")
        {
            logger->info(docWriter.generateAppVersionString());
            return true;
        }
    }
    return false;
}

inline bool CliApp::commandShortCircuits(std::vector<std::string> &args,
                                  const cli::commands::Command *cmd) const
{
    if (args.size() == 1 && (args.at(0) == "-h" || args.at(0) == "--help"))
    {
        logger->info(std::string(docWriter.generateCommandDocString(*cmd)));
        return true;
    }
    return false;
}

inline CliApp &CliApp::withCommand(commands::Command &&subCommand)
{
    return withCommand(std::make_unique<commands::Command>(std::move(subCommand)));
}

} // namespace cli

#ifdef CHAIN_CLI_VERBOSE
#include <iostream>
#endif

namespace cli
{

inline bool CliContext::isOptionArgPresent(const std::string &argName) const
{
    return optionArgs->contains(argName);
}

inline bool CliContext::isPositionalArgPresent(const std::string &argName) const
{
    return positionalArgs->contains(argName);
}

inline bool CliContext::isFlagPresent(const std::string &argName) const
{
    return flagArgs->contains(argName);
}

inline bool CliContext::isArgPresent(const std::string &argName) const
{
    return isOptionArgPresent(argName) || isFlagPresent(argName) || isPositionalArgPresent(argName);
}

} // namespace cli

namespace cli::commands
{
inline bool ArgumentGroup::isRequired() const
{
    if (exclusive)
    {
        return std::ranges::all_of(arguments, [](auto const &arg) { return arg->isRequired(); });
    }
    if (inclusive)
    {
        return std::ranges::any_of(arguments, [](auto const &arg) { return arg->isRequired(); });
    }
    return false;
}
} // namespace cli::commands

namespace cli::commands::docwriting
{
    /// @brief Exception thrown when documentation strings of a command are not built.
class DocsNotBuildException : public std::runtime_error
{
public:
    /// @brief Construct a DocsNotBuildException with a message and command
    /// @param message The error message
    /// @param command The command whose docs weren't built
    DocsNotBuildException(const std::string &message, const Command &command)
        : std::runtime_error(message), command(command)
    {
    }

    /// @brief Gets the command whose documentation wasn't built
    /// @return reference to the command
    const Command &getCommand() const noexcept { return command; }

private:
    const Command &command;
};
} // namespace cli::commands::docwriting

namespace cli::commands
{

inline std::string_view Command::getDocStringShort() const
{
    if (docStringShort.empty())
    {
        throw docwriting::DocsNotBuildException(
            std::format("Short documentation string not built for command '{}'.", identifier), *this);
    }
    return docStringShort;
}

inline std::string_view Command::getDocStringLong() const
{
    if (docStringLong.empty())
    {
        throw docwriting::DocsNotBuildException(
            std::format("Long documentation string not built for command '{}'.", identifier), *this);
    }
    return docStringLong;
}

inline void Command::execute(const CliContext &context) const
{
    if (executePtr && *executePtr)
    {
        (*executePtr)(context);
    }
    else
    {
        // cli::CLI().Logger().error("Command {} currently has no execute function",
        // identifier);
    }
}

inline Command &Command::withShortDescription(std::string_view desc)
{
    shortDescription = desc;
    return *this;
}

inline Command &Command::withLongDescription(std::string_view desc)
{
    longDescription = desc;
    return *this;
}

inline Command &Command::withFlagArgument(std::shared_ptr<FlagArgument> arg)
{
    safeAddToArgGroup(arg);
    flagArguments.push_back(arg);
    return *this;
}

inline Command &Command::withFlagArgument(FlagArgument &&arg)
{
    return withFlagArgument(std::make_shared<FlagArgument>(std::move(arg)));
}

inline Command &Command::withFlagArgument(FlagArgument &arg)
{
    return withFlagArgument(std::make_shared<FlagArgument>(arg));
}

inline Command &Command::withExecutionFunc(
    std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
{
    executePtr = std::move(actionPtr);
    return *this;
}

inline Command &Command::withExecutionFunc(std::function<void(const CliContext &)> &&action)
{
    return withExecutionFunc(
        std::make_unique<std::function<void(const CliContext &)>>(std::move(action)));
}

inline Command &Command::withSubCommand(std::unique_ptr<Command> subCommandPtr)
{
    subCommands.try_emplace(subCommandPtr->identifier, std::move(subCommandPtr));
    return *this;
}

inline Command &Command::withSubCommand(Command &&subCommand)
{
    return withSubCommand(std::make_unique<Command>(std::move(subCommand)));
}

inline void Command::safeAddToArgGroup(const std::shared_ptr<ArgumentBase> &arg)
{
    if (indexForNewArgGroup >= argumentGroups.size())
    {
        argumentGroups.push_back(std::make_unique<ArgumentGroup>(false, false));
    }
    argumentGroups[indexForNewArgGroup]->addArgument(arg);
}

inline void Command::addArgGroup(const ArgumentGroup &argGroup)
{
    for (auto &arg : argGroup.getArguments())
    {
        switch (arg->getArgType())
        {
        case ArgumentKind::Flag:
            flagArguments.push_back(std::static_pointer_cast<FlagArgument>(arg));
            break;

        case ArgumentKind::Positional:
            positionalArguments.push_back(std::static_pointer_cast<PositionalArgumentBase>(arg));
            break;

        case ArgumentKind::Option:
            optionArguments.push_back(std::static_pointer_cast<OptionArgumentBase>(arg));
            break;
        }
    }
}

inline Command *Command::getSubCommand(std::string_view id)
{
    auto it = subCommands.find(id);
    return (it != subCommands.end()) ? it->second.get() : nullptr;
}

inline const Command *Command::getSubCommand(std::string_view id) const
{
    auto it = subCommands.find(id);
    return (it != subCommands.end()) ? it->second.get() : nullptr;
}

inline std::ostream &operator<<(std::ostream &out, const Command &cmd)
{
    out << "Command - " << cmd.identifier << " ( short Description: " << cmd.shortDescription
        << "; long Description: " << cmd.longDescription << "; arguments: [";

    for (size_t i = 0; i < cmd.positionalArguments.size(); ++i)
    {
        // if (cmd.arguments[i])
        // out << *(cmd.arguments[i]); // assumes Argument has operator<<
        if (i + 1 < cmd.positionalArguments.size())
            out << ", ";
    }

    out << "])";
    return out;
}

inline std::string MalformedCommandException::buildMessage(const Command &cmd, const std::string &msg)
{
    std::ostringstream oss;
    oss << "Malformed Command: " << cmd.getIdentifier();
    if (!msg.empty())
        oss << " - " << msg;
    return oss.str();
}

} // namespace cli::commands

namespace cli::commands
{
inline CommandTree::CommandTree(std::string_view rootName)
: root(std::make_unique<Command>(rootName))
{
}

inline std::string_view CommandTree::getPathForCommand(Command *cmd) const
{
    return commandPathMap.at(cmd);
}

inline void CommandTree::buildCommandPathMap(const std::string &separator)
{
    std::unordered_map<Command *, std::string> map;
    if (root)
    {
        std::vector<std::string> path;
        buildCommandPathMapRecursive(root.get(), path, separator);
    }
}

inline void CommandTree::buildCommandPathMapRecursive(Command *cmd, std::vector<std::string> &path,
                                               const std::string &separator)
{
    path.emplace_back(cmd->getIdentifier());

    // Build the full path string
    std::string fullPath;
    for (size_t i = 0; i < path.size(); ++i)
    {
        fullPath += path[i];
        if (i + 1 < path.size())
            fullPath += separator;
    }

    commandPathMap[cmd] = fullPath;

    // Recurse into subcommands
    for (const auto &[key, value] : cmd->getSubCommands())
    {
        buildCommandPathMapRecursive(value.get(), path, separator);
    }

    path.pop_back();
}

inline std::string CommandNotFoundException::buildMessage(const std::string &id,
                                                   const std::vector<std::string> &chain)
{
    std::ostringstream oss;
    oss << "Parent command not found: '" << id << "' in path [";
    for (size_t i = 0; i < chain.size(); ++i)
    {
        oss << chain[i];
        if (i + 1 < chain.size())
            oss << " -> ";
    }
    oss << "]";
    return oss.str();
}

inline std::vector<Command *> CommandTree::getAllCommands() const
{
    std::vector<Command *> commands;
    if (root)
    {
        getAllCommandsRecursive(root.get(), commands);
    }
    return commands;
}

inline std::vector<const Command *> CommandTree::getAllCommandsConst() const
{
    std::vector<const Command *> commands;
    if (root)
    {
        getAllCommandsRecursive(root.get(), commands);
    }
    return commands;
}
} // namespace cli::commands

namespace cli::commands::docwriting
{

inline std::pair<std::string, std::string> getPositionalArgumentBrackets(bool required)
{
    if (required)
        return {"<", ">"};
    else
        return {"[<", ">]"};
}

inline std::pair<char, char> getOptionArgumentBrackets(bool required)
{
    if (required)
        return {'(', ')'};
    else
        return {'[', ']'};
}

inline std::string DefaultFlagFormatter::generateArgDocString(
    const FlagArgument &argument, [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getOptionArgumentBrackets(argument.isRequired());
    builder << inBracket << argument.getName();
    if (!argument.getShortName().empty())
    {
        builder << ',' << argument.getShortName();
    }
    builder << outBracket;
    return builder.str();
}

inline std::string DefaultFlagFormatter::generateOptionsDocString(
    const FlagArgument &argument, [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    builder << argument.getName();
    if (!argument.getShortName().empty())
    {
        builder << ' ' << argument.getShortName();
    }
    return std::format("{:<{}}{:>{}}", builder.str(), configuration.optionsWidth,
                       argument.getOptionComment(), argument.getOptionComment().size());
}

inline std::string DefaultOptionFormatter::generateArgDocString(
    const OptionArgumentBase &argument, [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getOptionArgumentBrackets(argument.isRequired());
    builder << inBracket << argument.getName();
    if (!argument.getShortName().empty())
    {
        builder << ',' << argument.getShortName();
    }
    builder << ' ' << '<' << argument.getValueName() << '>' << outBracket;

    if (argument.isRepeatable())
        builder << "...";
    return builder.str();
}

inline std::string DefaultOptionFormatter::generateOptionsDocString(
    const OptionArgumentBase &argument, [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    builder << argument.getName();
    if (!argument.getShortName().empty())
    {
        builder << ',' << argument.getShortName();
    }
    builder << ' ' << '<' << argument.getValueName() << '>';
    if (argument.isRepeatable())
        builder << "...";
    return std::format("{:<{}}{:>{}}", builder.str(), configuration.optionsWidth,
                       argument.getOptionComment(), argument.getOptionComment().size());
}

inline std::string DefaultPositionalFormatter::generateArgDocString(
    const PositionalArgumentBase &argument, [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getPositionalArgumentBrackets(argument.isRequired());

    builder << inBracket << argument.getName();
    builder << outBracket;

    if (argument.isRepeatable())
        builder << "...";
    return builder.str();
}

inline std::string DefaultPositionalFormatter::generateOptionsDocString(
    const PositionalArgumentBase &argument, [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getPositionalArgumentBrackets(argument.isRequired());

    builder << inBracket << argument.getName() << outBracket;
    if (argument.isRepeatable())
        builder << "...";
    return std::format("{:<{}}{:>{}}", builder.str(), configuration.optionsWidth,
                       argument.getOptionComment(), argument.getOptionComment().size());
}

inline void addGroupArgumentDocString(std::ostringstream &builder,
                                        const cli::commands::ArgumentGroup &groupArgs,
                                        const DocWriter &writer)
{
    auto [inBracket, outBracket] = getOptionArgumentBrackets(groupArgs.isRequired());
    if (groupArgs.isExclusive() || groupArgs.isInclusive())
    {
        builder << inBracket;
    }

    auto args = groupArgs.getArguments();
    for (size_t i = 0; i < args.size(); ++i)
    {
        const auto &argPtr = args[i];
        builder << argPtr->getArgDocString(writer);

        if (i < args.size() - 1) // not the last element
        {
            if (groupArgs.isExclusive())
                builder << " | ";
            else
                builder << " ";
        }
    }

    if (groupArgs.isExclusive() || groupArgs.isInclusive())
    {
        builder << outBracket;
    }
}

inline std::string DefaultCommandFormatter::generateLongDocString(
    const Command &command, std::string_view fullCommandPath, const DocWriter &writer,
    [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    builder << fullCommandPath << " ";

    for (const auto &argGroupPtr : command.getArgumentGroups())
    {
        addGroupArgumentDocString(builder, *argGroupPtr, writer);
        builder << ' ';
    }

    builder << "\n\n" << command.getLongDescription() << "\n\nOptions:\n";

    for (const auto &argGroupPtr : command.getArgumentGroups())
    {
        for (const auto &argPtr : argGroupPtr->getArguments())
        {
            builder << argPtr->getOptionsDocString(writer) << "\n";
        }
    }
    return builder.str();
}

inline std::string DefaultCommandFormatter::generateShortDocString(
    const Command &command, std::string_view fullCommandPath, const DocWriter &writer,
    [[maybe_unused]] const cli::CliConfig &configuration)
{
    std::ostringstream builder;
    builder << fullCommandPath << " ";

    for (const auto &argGroupPtr : command.getArgumentGroups())
    {
        addGroupArgumentDocString(builder, *argGroupPtr, writer);
        builder << " ";
    }
    builder << "\n" << command.getShortDescription();
    return builder.str();
}

inline std::string DefaultCliAppDocFormatter::generateAppDocString(
    const cli::CliConfig &configuration,
    const std::vector<const cli::commands::Command *> &commands)
{
    std::ostringstream builder;
    builder << configuration.description << "\n\n";

    if (commands.size() == 1)
    {
        auto cmd = commands.at(0);
        if (cmd->hasExecutionFunction())
                builder << cmd->getDocStringLong();
    }
    else
    {
        for (const auto &cmd : commands)
        {
            if (cmd->hasExecutionFunction())
                builder << cmd->getDocStringShort() << "\n\n";
        }
        builder << "Use <command> --help|-h to get more information about a specific command";
    }

    return builder.str();
}

inline std::string DefaultCliAppDocFormatter::generateCommandDocString(
    const Command &command, [[maybe_unused]] const cli::CliConfig &configuration)
{
    return std::string(command.getDocStringLong());
}

inline std::string DefaultCliAppDocFormatter::generateAppVersionString(const cli::CliConfig &configuration)
{
    return std::format("{} version: {}", configuration.executableName, configuration.version);
}

} // namespace cli::commands::docwriting

namespace cli::commands::docwriting
{

inline void DocWriter::setOptionFormatter(
    std::unique_ptr<AbstractArgDocFormatter<OptionArgumentBase>> formatter)
{
    optionFormatterPtr = std::move(formatter);
}

inline void DocWriter::setPositionalFormatter(
    std::unique_ptr<AbstractArgDocFormatter<PositionalArgumentBase>> formatter)
{
    positionalFormatterPtr = std::move(formatter);
}

inline void DocWriter::setFlagFormatter(std::unique_ptr<AbstractArgDocFormatter<FlagArgument>> formatter)
{
    flagFormatterPtr = std::move(formatter);
}

inline void DocWriter::setCommandFormatter(std::unique_ptr<AbstractCommandFormatter> formatter)
{
    commandFormatterPtr = std::move(formatter);
}

inline void DocWriter::setDocStrings(Command &command, std::string_view fullCommandPath) const
{
    command.docStringLong = generateLongDocString(command, fullCommandPath);
    command.docStringShort = generateShortDocString(command, fullCommandPath);
}

inline std::string DocWriter::generateShortDocString(const Command &command,
                                              std::string_view fullCommandPath) const
{
    return commandFormatterPtr->generateShortDocString(command, fullCommandPath, *this,
                                                       configuration);
}

inline std::string DocWriter::generateLongDocString(const Command &command,
                                             std::string_view fullCommandPath) const
{
    return commandFormatterPtr->generateLongDocString(command, fullCommandPath, *this,
                                                      configuration);
}

inline std::string DocWriter::generateOptionsDocString(const FlagArgument &argument) const
{
    return flagFormatterPtr->generateOptionsDocString(argument, configuration);
}

inline std::string DocWriter::generateArgDocString(const FlagArgument &argument) const
{
    return flagFormatterPtr->generateArgDocString(argument, configuration);
}

inline std::string DocWriter::generateOptionsDocString(const OptionArgumentBase &argument) const
{
    return optionFormatterPtr->generateOptionsDocString(argument, configuration);
}

inline std::string DocWriter::generateArgDocString(const OptionArgumentBase &argument) const
{
    return optionFormatterPtr->generateArgDocString(argument, configuration);
}

inline std::string DocWriter::generateOptionsDocString(const PositionalArgumentBase &argument) const
{
    return positionalFormatterPtr->generateOptionsDocString(argument, configuration);
}

inline std::string DocWriter::generateArgDocString(const PositionalArgumentBase &argument) const
{
    return positionalFormatterPtr->generateArgDocString(argument, configuration);
}

inline std::string DocWriter::generateAppDocString(const std::vector<const cli::commands::Command*> &commands) const
{
    return appFormatterPtr->generateAppDocString(configuration, commands);
}

inline std::string DocWriter::generateCommandDocString(const Command &command) const
{
    return appFormatterPtr->generateCommandDocString(command, configuration);
}

inline std::string DocWriter::generateAppVersionString() const
{
    return appFormatterPtr->generateAppVersionString(configuration);
}

inline void DocWriter::setAppFormatter(std::unique_ptr<AbstractCliAppDocFormatter> formatter)
{
    appFormatterPtr = std::move(formatter);
}

} // namespace cli::commands::docwriting

namespace cli::commands
{
inline std::string FlagArgument::getOptionsDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateOptionsDocString(*this);
}

inline std::string FlagArgument::getArgDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateArgDocString(*this);
}

} // namespace cli::commands

namespace cli::commands
{
inline std::string OptionArgumentBase::getOptionsDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateOptionsDocString(*this);
}

inline std::string OptionArgumentBase::getArgDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateArgDocString(*this);
}
} // namespace cli::commands

namespace cli::commands
{
inline std::string PositionalArgumentBase::getOptionsDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateOptionsDocString(*this);
}

inline std::string PositionalArgumentBase::getArgDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateArgDocString(*this);
}
} // namespace cli::commands

#ifdef CHAIN_CLI_VERBOSE
#include <iostream>
#endif

namespace cli
{
inline ContextBuilder::ContextBuilder()
    : positionalArgs(std::make_unique<std::unordered_map<std::string, std::any>>()),
      optionalArgs(std::make_unique<std::unordered_map<std::string, std::any>>()),
      flagArgs(std::make_unique<std::unordered_set<std::string>>())
{
}

inline ContextBuilder &ContextBuilder::addPositionalArgument(const std::string &argName, std::any &val)
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Appending positional argument: " << argName << "\n";
#endif
    positionalArgs->try_emplace(argName, val);
    return *this;
}

inline ContextBuilder &ContextBuilder::addPositionalArgument(std::string_view argName, std::any &val)
{
    return addPositionalArgument(std::string(argName), val);
}

inline ContextBuilder &ContextBuilder::addRepeatablePositionalArgument(const std::string &argName, const std::vector<std::any> &values)
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Adding repeatable positional argument to context: " << argName << " with " << values.size() << " values\n";
#endif
    if(!positionalArgs->contains(argName))
    {
        positionalArgs->try_emplace(argName, values);
    }
    else 
    {
        // If the argument already exists, we need to append the new values to the existing ones
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "Adding to existing repeatable positional argument: " << argName << "\n";
#endif
        std::any &existingValues = positionalArgs->at(argName);
        std::vector<std::any> &vec = std::any_cast<std::vector<std::any> &>(existingValues);
        vec.insert(vec.end(), values.begin(), values.end());
    }
    return *this;
}

inline ContextBuilder &ContextBuilder::addRepeatablePositionalArgument(std::string_view argName, const std::vector<std::any> &values)
{
    return addRepeatablePositionalArgument(std::string(argName), values);
}

inline ContextBuilder &ContextBuilder::addOptionArgument(const std::string &argName, std::any &val)
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Adding option argument: " << argName << "\n";
#endif
    optionalArgs->try_emplace(argName, val);
    return *this;
}

inline ContextBuilder &ContextBuilder::addOptionArgument(std::string_view argName, std::any &val)
{
    return addOptionArgument(std::string(argName), val);
}

inline ContextBuilder &ContextBuilder::addRepeatableOptionArgument(const std::string &argName, const std::vector<std::any> &values)
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Adding to repeatable option argument to context: " << argName << " with " << values.size() << " values\n";
#endif
    if(!optionalArgs->contains(argName))
    {
        optionalArgs->try_emplace(argName, values);
    }
    else
    {
        //append to existing values if already provided
#ifdef CHAIN_CLI_VERBOSE
        std::cout << "  Appending to existing repeatable option argument: " << argName << "\n";
#endif
        std::any &existingValues = optionalArgs->at(argName);
        std::vector<std::any> &vec = std::any_cast<std::vector<std::any> &>(existingValues);
        vec.insert(vec.end(), values.begin(), values.end());
    }
    return *this;
}

inline ContextBuilder &ContextBuilder::addRepeatableOptionArgument(std::string_view argName, const std::vector<std::any> &values)
{
    return addRepeatableOptionArgument(std::string(argName), values);
}

inline ContextBuilder &ContextBuilder::addFlagArgument(const std::string &argName)
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Adding flag argument: " << argName << "\n";
#endif
    flagArgs->insert(argName);
    return *this;
}

inline ContextBuilder &ContextBuilder::addFlagArgument(std::string_view argName)
{
    flagArgs->insert(std::string(argName));
    return *this;
}

inline bool ContextBuilder::isArgPresent(const std::string &argName) const
{
    return optionalArgs->contains(argName) || flagArgs->contains(argName) ||
           positionalArgs->contains(argName);
}

inline std::unique_ptr<CliContext> ContextBuilder::build(cli::logging::AbstractLogger &logger)
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Building CliContext with " << positionalArgs->size() << " positional, " 
              << optionalArgs->size() << " option, and " << flagArgs->size() << " flag arguments\n";
#endif
    return std::make_unique<CliContext>(std::move(positionalArgs), std::move(optionalArgs),
                                        std::move(flagArgs), logger);
}
} // namespace cli

namespace cli
{
    inline std::string MissingArgumentException::makeMessage(
    const std::string &name, const std::unordered_map<std::string, std::any> &args)
{
    std::ostringstream oss;
    oss << "Missing argument: \"" << name << "\" was not passed in this context.\n";
    oss << "Available arguments: ";
    if (args.empty())
    {
        oss << "<none>";
    }
    else
    {
        bool first = true;
        for (auto &[k, _] : args)
        {
            if (!first)
                oss << ", ";
            oss << k;
            first = false;
        }
    }
    return oss.str();
}

inline std::string InvalidArgumentTypeException::makeMessage(const std::string &name,
                                                      const std::type_info &requested,
                                                      const std::type_info &actual)
{
    std::ostringstream oss;
    oss << "Invalid type for argument: \"" << name << "\"\n"
        << "Requested type: " << requested.name() << "\n"
        << "Actual type: " << actual.name();
    return oss.str();
}
} //namespace cli

namespace cli::logging
{

inline std::string BasicFormatter::format(const LogRecord &record) const
{
    auto const time = timezone->to_local(record.timestamp);

    std::ostringstream oss;
    oss << "[" << std::format("{:%Y-%m-%d %X}", time) << "] " << toString(record.level) << ": "
        << record.message << "\n";
    return oss.str();
}

inline std::string MessageOnlyFormatter::format(const LogRecord &record) const
{
    return record.message + "\n";
}

} // namespace cli::logging

namespace cli::logging
{
inline BaseHandler::~BaseHandler()
{
    err.flush();
    out.flush();
}

inline void BaseHandler::emit(const LogRecord &record) const
{
    if (record.level < minLevel)
        return; // ignore messages below minimum level

    auto formatted = formatterPtr->format(record);

    if (stylingEnabled && styleMapPtr)
    {
        auto it = styleMapPtr->find(record.level);
        if (it != styleMapPtr->end())
        {
            formatted = it->second + formatted + "\033[0m"; // reset style
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

inline void BaseHandler::setStyleMap(std::shared_ptr<const LogStyleMap> styles)
{
    styleMapPtr = styles;
}

inline FileHandler::FileHandler(const std::string &filename, std::shared_ptr<AbstractFormatter> f,
                         LogLevel minLevel, std::shared_ptr<const LogStyleMap> styles)
    : BaseHandler(file, file, f, minLevel, std::move(styles)), file(filename, std::ios::app)
{
    if (!file.is_open())
    {
        throw std::ios_base::failure("Failed to open log file: " + filename);
    }
}

inline FileHandler::~FileHandler()
{
    if (file.is_open())
    {
        file.close(); // ensure the file is closed
    }
}
} // namespace cli::logging

namespace cli::logging
{
inline Logger::Logger(LogLevel lvl) : minLevel(lvl)
{
    // Wrap logInternal as a lambda and pass it to LogStreamBuf
    auto logFuncPtr = std::make_shared<std::function<void(LogLevel, const std::string &)>>(
        [this](LogLevel level, const std::string &msg) { this->log(level, msg); });

    for (auto i = static_cast<int>(LogLevel::TRACE); i <= static_cast<int>(LogLevel::ERROR); ++i)
    {
        auto level = static_cast<LogLevel>(i);
        buffers[level] = std::make_unique<LogStreamBuf>(logFuncPtr, level, minLevel);
        streams[level] = std::make_unique<std::ostream>(buffers[level].get());
    }

    // add default console handler
    addHandler(std::make_unique<ConsoleHandler>(std::make_shared<MessageOnlyFormatter>(), LogLevel::TRACE));
}

inline void Logger::setLevel(LogLevel lvl)
{
    minLevel = lvl;
    for (auto const &[level, buffer] : buffers)
    {
        buffer->setMinLevel(lvl);
    }
}

inline void Logger::addHandler(std::unique_ptr<AbstractHandler> handlerPtr)
{
    handlers.push_back(std::move(handlerPtr));
}

inline void Logger::log(LogLevel lvl, const std::string &msg) const
{
    if(lvl < minLevel)
        return; // ignore messages below minimum level
    LogRecord record{lvl, msg};

    for (auto const &handler : handlers)
    {
        handler->emit(record);
    }
}

inline std::ostream &Logger::getStream(LogLevel lvl)
{
    if (auto it = streams.find(lvl); it != streams.end())
    {
        return *(it->second);
    }
    throw std::invalid_argument("Invalid log level for stream");
}

} // namespace cli::logging

namespace cli::logging
{
inline int LogStreamBuf::sync()
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

// used by heady

namespace cli::parsing
{
inline std::vector<std::any> Parser::parseRepeatableList(const cli::commands::TypedArgumentBase &arg,
                                                  const std::string &input) const
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Parsing repeatable list for argument of type: " << arg.getType().name()
              << " with delimiter-separated input: " << input << "\n";
#endif
    std::stringstream ss(input);
    std::string token;
    std::vector<std::any> parsedValues;

    while (std::getline(ss, token, configuration.repeatableDelimiter))
    {
        // Trim leading whitespace
        token.erase(token.begin(), std::ranges::find_if(
                                       token, [](unsigned char ch) { return !std::isspace(ch); }));

        // Trim trailing whitespace
        token.erase(std::ranges::find_if(token | std::views::reverse,
                                         [](unsigned char ch) { return !std::isspace(ch); })
                        .base(),
                    token.end());

        if (!token.empty())
        {
            parsedValues.push_back(arg.parseToValue(token));
        }
    }

    return parsedValues;
}

inline void Parser::parseRepeatable(const cli::commands::OptionArgumentBase &arg, const std::string &input,
                             ContextBuilder &contextBuilder) const
{
    auto values = parseRepeatableList(arg, input);
    contextBuilder.addRepeatableOptionArgument(arg.getName(), values);
}

inline void Parser::parseRepeatable(const cli::commands::PositionalArgumentBase &arg,
                             const std::string &input, ContextBuilder &contextBuilder) const
{
    auto values = parseRepeatableList(arg, input);
    contextBuilder.addRepeatablePositionalArgument(arg.getName(), values);
}

inline bool Parser::tryOptionArg(
    const std::vector<std::shared_ptr<cli::commands::OptionArgumentBase>> &optionArguments,
    const std::vector<std::string> &inputs, const std::string &currentParsing, size_t index,
    ContextBuilder &contextBuilder) const
{
    const cli::commands::OptionArgumentBase *matchedOpt = nullptr;
    for (const auto &opt : optionArguments)
    {
        if (currentParsing == opt->getShortName() || currentParsing == opt->getName())
        {
            matchedOpt = opt.get();
            break;
        }
    }
    if (matchedOpt)
    {
        if (matchedOpt->isRepeatable())
        {
            parseRepeatable(*matchedOpt, inputs[index + 1], contextBuilder);
        }
        else
        {
            if (!matchedOpt->isRepeatable() &&
                contextBuilder.isArgPresent(std::string(matchedOpt->getName())))
            {
                throw ParseException(
                    std::format("Non Repeatable Argument {} was repeated", matchedOpt->getName()),
                    inputs[index + 1], *matchedOpt);
            }

            auto val = matchedOpt->parseToValue(inputs[index + 1]);
            contextBuilder.addOptionArgument(matchedOpt->getName(), val);
        }
        return true;
    }
    return false;
}

inline bool Parser::tryFlagArg(
    const std::vector<std::shared_ptr<cli::commands::FlagArgument>> &flagArguments,
    const std::string &currentParsing, ContextBuilder &contextBuilder) const
{
    const cli::commands::FlagArgument *matchedFlag = nullptr;
    for (const auto &flag : flagArguments)
    {
        if (currentParsing == flag->getShortName() || currentParsing == flag->getName())
        {
            matchedFlag = flag.get();
            break;
        }
    }
    if (matchedFlag)
    {
        contextBuilder.addFlagArgument(matchedFlag->getShortName());
        contextBuilder.addFlagArgument(matchedFlag->getName());
        return true;
    }
    return false;
}

inline void cli::parsing::Parser::parseArguments(const cli::commands::Command &command,
                                          const std::vector<std::string> &inputs,
                                          ContextBuilder &contextBuilder) const
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Parsing arguments for command: " << command.getIdentifier() << "\n";
    std::cout << "Input arguments: ";
    for (const auto &input : inputs)
    {
        std::cout << "\"" << input << "\" ";
    }
    std::cout << "\n";
#endif

    const auto &posArguments = command.getPositionalArguments();
    const auto &optArguments = command.getOptionArguments();
    const auto &flagArguments = command.getFlagArguments();

    size_t posArgsIndex = 0;
    for (size_t i = 0; i < inputs.size(); ++i)
    {
        const auto &input = inputs[i];
        if (tryOptionArg(optArguments, inputs, input, i, contextBuilder))
        {
#ifdef CHAIN_CLI_VERBOSE
            std::cout << "Processed option argument: " << input << "\n";
#endif
            i++;
            continue;
        }

        if (tryFlagArg(flagArguments, input, contextBuilder))
        {
#ifdef CHAIN_CLI_VERBOSE
            std::cout << "Processed flag argument: " << input << "\n";
#endif
            continue;
        }

        if (posArgsIndex >= posArguments.size())
        {
            throw ParseException(std::format("More positional arguments were provided than the "
                                             "command accepts with input: {}",
                                             input),
                                 input, *(posArguments.back()));
        }

        if (const auto &posArg = *posArguments.at(posArgsIndex); posArg.isRepeatable())
        {
            parseRepeatable(posArg, input, contextBuilder);
        }
        else
        {
            if (!posArg.isRepeatable() &&
                contextBuilder.isArgPresent(std::string(posArg.getName())))
            {
                throw ParseException(
                    std::format("Non Repeatable Argument {} was repeated", posArg.getName()), input,
                    posArg);
            }
            auto val = posArg.parseToValue(input);
#ifdef CHAIN_CLI_VERBOSE
            std::cout << "Processed positional argument: " << input << "\n";
#endif
            contextBuilder.addPositionalArgument(posArg.getName(), val);
        }

        ++posArgsIndex;
    }
    checkGroupsAndRequired(command, contextBuilder);
}

inline void exclusiveCheck(const commands::ArgumentGroup *argGroup,
                             const ContextBuilder &contextBuilder)
{
    const cli::commands::ArgumentBase *firstProvided = nullptr;

    for (const auto &argPtr : argGroup->getArguments())
    {
        if (!firstProvided && contextBuilder.isArgPresent(std::string(argPtr->getName())))
        {
            firstProvided = argPtr.get();
        }
        else if (contextBuilder.isArgPresent(std::string(argPtr->getName())) &&
                 firstProvided != nullptr)
        {
            throw GroupParseException(
                std::format("Two arguments of mutually exclusive group were present: {} and {}",
                            firstProvided->getName(), argPtr->getName()),
                *argGroup);
        }
    }
}

inline void inclusiveCheck(const commands::ArgumentGroup *argGroup,
                             const ContextBuilder &contextBuilder)
{
    const cli::commands::ArgumentBase *firstProvided = nullptr;

    for (const auto &argPtr : argGroup->getArguments())
    {
        if (!firstProvided && contextBuilder.isArgPresent(std::string(argPtr->getName())))
        {
            firstProvided = argPtr.get();
        }
        else if (!contextBuilder.isArgPresent(std::string(argPtr->getName())))
        {
            throw GroupParseException(
                std::format("Missing argument in mutually exclusive group: {}", argPtr->getName()),
                *argGroup);
        }
    }
}

inline void checkRequired(const commands::ArgumentGroup *argGroup,
                            const ContextBuilder &contextBuilder)
{
    for (const auto &argPtr : argGroup->getArguments())
    {
        if (argPtr->isRequired() && !contextBuilder.isArgPresent(std::string(argPtr->getName())))
        {
            throw ParseException(std::format("Required argument {} is missing", argPtr->getName()),
                                 "", *argPtr);
        }
    }
}

inline void Parser::checkGroupsAndRequired(const cli::commands::Command &command,
                                    const ContextBuilder &contextBuilder) const
{
    for (const auto &argGroup : command.getArgumentGroups())
    {
        if (argGroup->isExclusive())
        {
            exclusiveCheck(argGroup.get(), contextBuilder);
        }
        else if (argGroup->isInclusive())
        {
            inclusiveCheck(argGroup.get(), contextBuilder);
        }
        checkRequired(argGroup.get(), contextBuilder);
    }
}

} // namespace cli::parsing