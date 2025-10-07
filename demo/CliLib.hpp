
// Amalgamation-specific define
#ifndef HEADY_HEADER_ONLY
#define HEADY_HEADER_ONLY
#endif


// begin --- cli_base.cpp --- 



// begin --- cli_base.h --- 

#pragma once
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

// begin --- cli_config.h --- 

#pragma once
#include <string>

namespace cli
{
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

// end --- cli_config.h --- 



// begin --- command_tree.h --- 

#pragma once
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string_view>
#include <unordered_map>

// begin --- command.h --- 

#pragma once

// begin --- argument_group.h --- 

#pragma once
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

// begin --- argument.h --- 

#pragma once
#include <any>
#include <string>
#include <string_view>
#include <typeindex>

namespace cli::commands
{
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

class ArgumentBase
{
public:
    virtual ~ArgumentBase() = default;

    [[nodiscard]] constexpr std::string_view getName() const noexcept { return name; }

    [[nodiscard]] constexpr std::string_view getUsageComment() const noexcept
    {
        return optionsComment;
    }

    [[nodiscard]] constexpr bool isRequired() const noexcept { return required; }

    [[nodiscard]] constexpr bool isRepeatable() const noexcept { return repeatable; }

    [[nodiscard]] constexpr ArgumentKind getArgType() const { return argType; }

    [[nodiscard]] virtual std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const = 0;
    [[nodiscard]] virtual std::string getArgDocString(
        const docwriting::DocWriter &writer) const = 0;

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

class TypedArgumentBase
{
public:
    virtual ~TypedArgumentBase() = default;

    [[nodiscard]] std::type_index getType() const { return type; }

    [[nodiscard]] virtual std::any parseToValue(const std::string &input) const = 0;

protected:
    explicit TypedArgumentBase(std::type_index t) : type(t) {}

    const std::type_index type;
};

class FlaggedArgumentBase
{
public:
    [[nodiscard]] constexpr std::string_view getShortName() const noexcept { return shortName; }

protected:
    explicit FlaggedArgumentBase(std::string_view shortName) : shortName(shortName) {}

    std::string shortName;
};
} // namespace cli::commands

// end --- argument.h --- 



namespace cli::commands
{
class ArgumentGroup
{
public:
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

    [[nodiscard]] const std::vector<std::shared_ptr<ArgumentBase>> &getArguments() const noexcept
    {
        return arguments;
    }

    [[nodiscard]] bool isExclusive() const noexcept { return exclusive; }

    [[nodiscard]] bool isInclusive() const noexcept { return inclusive; }

    bool isRequired() const;

    void addArgument(const std::shared_ptr<ArgumentBase> &arg) { arguments.push_back(arg); }

private:
    std::vector<std::shared_ptr<ArgumentBase>> arguments;
    bool exclusive;
    bool inclusive;
};

class ExclusiveGroup : public ArgumentGroup
{
public:
    template <typename... Args>
        requires((!std::same_as<std::remove_cvref_t<Args>, ExclusiveGroup> && ...) &&
                 (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
    explicit ExclusiveGroup(Args &&...args)
        : ArgumentGroup(true, false, std::forward<Args>(args)...)
    {
    }
};

class InclusiveGroup : public ArgumentGroup
{
public:
    template <typename... Args>
        requires((!std::same_as<std::remove_cvref_t<Args>, InclusiveGroup> && ...) &&
                 (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
    explicit InclusiveGroup(Args &&...args)
        : ArgumentGroup(false, true, std::forward<Args>(args)...)
    {
    }
};
} // namespace cli::commands


// end --- argument_group.h --- 



// begin --- cli_context.h --- 

#pragma once
#include <any>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>

// begin --- logger.h --- 

#pragma once
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

// begin --- handler.h --- 

#pragma once
#include <fstream>
#include <iostream>
#include <memory>

// begin --- formatter.h --- 

#pragma once
#include <chrono>
#include <string>

// begin --- logrecord.h --- 

#pragma once
#include <chrono>
#include <string>

// begin --- loglevel.h --- 

#pragma once
#include <string_view>

namespace cli::logging
{

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

// end --- loglevel.h --- 



namespace cli::logging
{

struct LogRecord
{
    const LogLevel level;
    const std::string message;
    const std::chrono::system_clock::time_point timestamp{std::chrono::system_clock::now()};

    // Constructor initializes all fields; fields are immutable
    LogRecord(LogLevel lvl, std::string msg) : level(lvl), message(std::move(msg)) {}
};

} // namespace cli::logging

// end --- logrecord.h --- 



namespace cli::logging
{

class IFormatter
{
public:
    virtual ~IFormatter() = default;
    virtual std::string format(const LogRecord &record) const = 0;
};

// A simple formatter with timestamp + level + message
class BasicFormatter : public IFormatter
{
public:
    std::string format(const LogRecord &record) const override;

private:
    const std::chrono::time_zone *timezone = std::chrono::current_zone();
};

// A formatter that displays only the message
class MessageOnlyFormatter : public IFormatter
{
public:
    std::string format(const LogRecord &record) const override;
};

} // namespace cli::logging

// end --- formatter.h --- 



// begin --- logstyle.h --- 

#pragma once
#include <map>
#include <string>

namespace cli::logging
{

// contains ANSI escape codes for styling log messages
using LogStyleMap = std::map<LogLevel, std::string>;

// Default console styles
inline LogStyleMap defaultStyles()
{
    using enum cli::logging::LogLevel;
    return {
        {TRACE, "\o{33}[90m"},   // gray
        {VERBOSE, "\o{33}[90m"}, // gray
        {DEBUG, "\o{33}[36m"},   // cyan
        // Info uses plain grey text (no color)
        {SUCCESS, "\o{33}[32m"}, // green
        {WARNING, "\o{33}[33m"}, // yellow
        {ERROR, "\o{33}[31m"},   // red
    };
}
} // namespace cli::logging

// end --- logstyle.h --- 



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


// end --- handler.h --- 



namespace cli::logging
{
class LogStreamBuf : public std::stringbuf
{
public:
    LogStreamBuf(std::shared_ptr<std::function<void(LogLevel, const std::string &)>> logFuncPtr,
                 LogLevel lvl, LogLevel lvlMin)
        : logFuncPtr(logFuncPtr), lvl(lvl), minLevel(lvlMin)
    {
    }

    int sync() override;

    void setMinLevel(LogLevel lvlMin) { minLevel = lvlMin; }

private:
    std::shared_ptr<std::function<void(LogLevel, const std::string &)>> logFuncPtr;
    LogLevel lvl;
    LogLevel minLevel;
};

class Logger
{
public:
    // Non-copyable
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    Logger(Logger &&) = default;
    Logger &operator=(Logger &&) = default;

    explicit Logger(LogLevel lvl = LogLevel::TRACE);

    void setLevel(LogLevel lvl);

    void addHandler(std::unique_ptr<IHandler> handlerPtr);

    void removeAllHandlers() { handlers.clear(); }

    template <typename... Args> void log(LogLevel lvl, const std::string &fmt, Args &&...args) const
    {
        if (lvl < minLevel)
            return;
        std::string formatted = std::vformat(fmt, std::make_format_args(args...));
        logInternal(lvl, formatted);
    }

    // Convenience helpers
    template <typename... Args> void trace(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::TRACE, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args> void verbose(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::VERBOSE, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args> void debug(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::DEBUG, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args> void success(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::SUCCESS, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args> void info(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::INFO, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args> void warning(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::WARNING, fmt, std::forward<Args>(args)...);
    }

    template <typename... Args> void error(const std::string &fmt, Args &&...args)
    {
        log(LogLevel::ERROR, fmt, std::forward<Args>(args)...);
    }

    std::ostream &getStream(LogLevel lvl) { return *streams[lvl]; }

    std::ostream &trace() { return *streams[LogLevel::TRACE]; }

    std::ostream &verbose() { return *streams[LogLevel::VERBOSE]; }

    std::ostream &debug() { return *streams[LogLevel::DEBUG]; }

    std::ostream &success() { return *streams[LogLevel::SUCCESS]; }

    std::ostream &info() { return *streams[LogLevel::INFO]; }

    std::ostream &warning() { return *streams[LogLevel::WARNING]; }

    std::ostream &error() { return *streams[LogLevel::ERROR]; }

private:
    void logInternal(LogLevel lvl, const std::string &fmt) const;
    LogLevel minLevel;
    std::vector<std::unique_ptr<IHandler>> handlers;

    // Per-level stream buffers & streams
    std::unordered_map<LogLevel, std::unique_ptr<LogStreamBuf>> buffers;
    std::unordered_map<LogLevel, std::unique_ptr<std::ostream>> streams;
};
} // namespace cli::logging

// end --- logger.h --- 



namespace cli
{
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

class CliContext
{
public:
    explicit CliContext(std::unique_ptr<std::unordered_map<std::string, std::any>> posArgs,
                        std::unique_ptr<std::unordered_map<std::string, std::any>> optArgs,
                        std::unique_ptr<std::unordered_set<std::string>> flagArgs,
                        cli::logging::Logger &logger)
        : Logger(logger), positionalArgs(std::move(posArgs)), optionArgs(std::move(optArgs)),
          flagArgs(std::move(flagArgs))
    {
    }

    // Non-copyable
    CliContext(const CliContext &) = delete;
    CliContext &operator=(const CliContext &) = delete;

    bool isArgPresent(const std::string &argName) const;
    bool isArgPresent(std::string_view argName) const;
    bool isOptionArgPresent(const std::string &argName) const;
    bool isPositionalArgPresent(const std::string &argName) const;
    bool isFlagPresent(const std::string &argName) const;

    template <typename T> T getPositionalArg(const std::string &argName) const
    {
        return getAnyCast<T>(argName, *positionalArgs);
    }

    template <typename T> void getPositionalArg(const std::string &argName, T &out) const
    {
        out = getAnyCast<T>(argName, *positionalArgs);
    }

    template <typename T> T getOptionArg(const std::string &argName) const
    {
        return getAnyCast<T>(argName, *optionArgs);
    }

    template <typename T> void getOptionArg(const std::string &argName, T &out) const
    {
        out = getAnyCast<T>(argName, *optionArgs);
    }

    template <typename T> std::vector<T> getRepeatableOptionArg(const std::string &argName) const
    {
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

    template <typename T>
    std::vector<T> getRepeatablePositionalArg(const std::string &argName) const
    {
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

    template <typename T> T getArg(const std::string &argName) const
    {
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

    template <typename T> auto getRepeatableArg(const std::string &argName) const
    {
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

    cli::logging::Logger &Logger;

private:
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

class ContextBuilder
{
public:
    ContextBuilder();

    ContextBuilder &addPositionalArgument(const std::string &argName, std::any &val);
    ContextBuilder &addPositionalArgument(std::string_view argName, std::any &val);
    ContextBuilder &addOptionArgument(const std::string &argName, std::any &val);
    ContextBuilder &addOptionArgument(std::string_view argName, std::any &val);
    ContextBuilder &addFlagArgument(const std::string &argName);
    ContextBuilder &addFlagArgument(std::string_view argName);

    bool isArgPresent(const std::string &argName) const;

    std::unique_ptr<CliContext> build(cli::logging::Logger &logger);

private:
    std::unique_ptr<std::unordered_map<std::string, std::any>> positionalArgs;
    std::unique_ptr<std::unordered_map<std::string, std::any>> optionalArgs;
    std::unique_ptr<std::unordered_set<std::string>> flagArgs;
};
} // namespace cli

// end --- cli_context.h --- 



// begin --- flag_argument.h --- 

#pragma once
#include <any>
#include <ostream>
#include <string>
#include <typeindex>

// begin --- parser_utils.h --- 

#pragma once
#include <iostream>
#include <sstream>
#include <string>

// begin --- parse_exception.h --- 

#pragma once
#include <stdexcept>

namespace cli::parsing
{
class ParseException : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};

class GroupParseException : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};

} // namespace cli::parsing

// end --- parse_exception.h --- 



namespace cli::parsing
{
struct ParseHelper
{
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
                throw ParseException("Failed to parse value from input: " + input);
            }
        }

        return value;
    }

    template <typename T> static void parse(const std::string &input, T &value)
    {
        // Call the return-by-value version and assign
        value = parse<T>(input);
    }
};
} // namespace cli::parsing

// end --- parser_utils.h --- 



namespace cli::commands
{
class FlagArgument : public ArgumentBase, public FlaggedArgumentBase
{
public:
    explicit FlagArgument(std::string_view name, std::string_view shortName = "",
                          std::string_view optionsComment = "", bool isRequired = false)
        : ArgumentBase(name, optionsComment, ArgumentKind::Flag, false, isRequired),
          FlaggedArgumentBase(shortName)
    {
    }

    [[nodiscard]] std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const override;
    [[nodiscard]] std::string getArgDocString(const docwriting::DocWriter &writer) const override;

    FlagArgument &withOptionsComment(std::string_view usage_comment)
    {
        optionsComment = usage_comment;
        return *this;
    }

    FlagArgument &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    FlagArgument &withShortName(std::string_view shortName)
    {
        this->shortName = shortName;
        return *this;
    }
};
} // namespace cli::commands

// end --- flag_argument.h --- 



// begin --- option_argument.h --- 

#pragma once
#include <any>
#include <ostream>
#include <string>
#include <string_view>
#include <typeindex>

namespace cli::commands
{
class OptionArgumentBase : public TypedArgumentBase, public ArgumentBase, public FlaggedArgumentBase
{
public:
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

template <typename T> class OptionArgument : public OptionArgumentBase
{
    friend std::ostream &operator<<(std::ostream &out, const OptionArgument<T> &arg)
    {
        out << arg.name << " (" << arg.optionsComment << ")";
        return out;
    }

public:
    explicit OptionArgument(std::string_view name, std::string_view valueName,
                            std::string_view shortName = "", std::string_view optionsComment = "",
                            bool required = false, bool repeatable = false)
        : OptionArgumentBase(name, optionsComment, repeatable, required, typeid(T), shortName,
                             valueName)
    {
    }

    [[nodiscard]] std::any parseToValue(const std::string &input) const override;

    OptionArgument<T> &withOptionsComment(std::string_view optionsComment)
    {
        this->optionsComment = optionsComment;
        return *this;
    }

    OptionArgument<T> &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    OptionArgument<T> &withShortName(std::string_view shortName)
    {
        this->shortName = shortName;
        return *this;
    }

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

// end --- option_argument.h --- 



// begin --- positional_argument.h --- 

#pragma once
#include <any>
#include <ostream>
#include <string>
#include <string_view>
#include <typeindex>

namespace cli::commands
{
// untemplated base class for storing in same STL
class PositionalArgumentBase : public TypedArgumentBase, public ArgumentBase
{
public:
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

template <typename T> class PositionalArgument : public PositionalArgumentBase
{
    friend std::ostream &operator<<(std::ostream &out, const PositionalArgument<T> &arg)
    {
        out << arg.name << " (" << arg.optionsComment << ")";
        return out;
    }

public:
    explicit PositionalArgument(std::string_view name, std::string_view optionsComment = "",
                                bool required = true, bool repeatable = false)
        : PositionalArgumentBase(name, optionsComment, repeatable, required, typeid(T))
    {
    }

    [[nodiscard]] std::any parseToValue(const std::string &input) const override;

    PositionalArgument<T> &withOptionsComment(std::string_view usage_comment)
    {
        optionsComment = usage_comment;
        return *this;
    }

    PositionalArgument<T> &withRequired(bool req)
    {
        required = req;
        return *this;
    }

    PositionalArgument<T> &withShortName(std::string_view shortName)
    {
        this->shortName = shortName;
        return *this;
    }

    PositionalArgument<T> &withRepeatable(bool rep)
    {
        repeatable = rep;
        return *this;
    }
};

template <typename T>
inline std::any PositionalArgument<T>::parseToValue(const std::string &input) const
{
    return cli::parsing::ParseHelper::parse<T>(input);
}
} // namespace cli::commands

// end --- positional_argument.h --- 


#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <string_view>

namespace cli::commands
{

class Command
{
    friend std::ostream &operator<<(std::ostream &out, const Command &cmd);

public:
    friend class cli::commands::docwriting::DocWriter;

    // Constructor initializes the identifier and description
    Command(std::string_view id, std::string_view short_desc, std::string_view long_desc,
            std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
        : identifier(id), shortDescription(short_desc), longDescription(long_desc),
          executePtr(std::move(actionPtr))
    {
    }

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

    [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept { return identifier; }

    [[nodiscard]] constexpr std::string_view getShortDescription() const noexcept
    {
        return shortDescription;
    }

    [[nodiscard]] constexpr std::string_view getLongDescription() const noexcept
    {
        return longDescription;
    }

    [[nodiscard]] constexpr bool hasExecutionFunction() const noexcept { return executePtr.get(); }

    [[nodiscard]] const std::vector<std::shared_ptr<PositionalArgumentBase>> &
    getPositionalArguments() const noexcept
    {
        return positionalArguments;
    }

    [[nodiscard]] const std::vector<std::shared_ptr<OptionArgumentBase>> &getOptionArguments()
        const noexcept
    {
        return optionArguments;
    }

    [[nodiscard]] const std::vector<std::shared_ptr<FlagArgument>> &getFlagArguments()
        const noexcept
    {
        return flagArguments;
    }

    [[nodiscard]] const std::vector<std::unique_ptr<ArgumentGroup>> &getArgumentGroups()
        const noexcept
    {
        return argumentGroups;
    }

    [[nodiscard]] std::string_view getDocStringShort() const;
    [[nodiscard]] std::string_view getDocStringLong() const;

    [[nodiscard]] Command *getSubCommand(std::string_view id);
    [[nodiscard]] const Command *getSubCommand(std::string_view id) const;

    [[nodiscard]] auto &getSubCommands() { return subCommands; }
    [[nodiscard]] auto const &getSubCommands() const { return subCommands; }

#pragma endregion Accessor

    // try to run the passed callable
    void execute(const CliContext &context) const;

#pragma region ChainingMethods
    Command &withShortDescription(std::string_view desc);
    Command &withLongDescription(std::string_view desc);

    template <typename T>
    Command &withPositionalArgument(std::shared_ptr<PositionalArgument<T>> arg)
    {
        safeAddToArgGroup(arg);
        positionalArguments.push_back(arg);
        return *this;
    }

    template <typename T> Command &withPositionalArgument(PositionalArgument<T> &&arg)
    {
        return withPositionalArgument(std::make_shared<PositionalArgument<T>>(std::move(arg)));
    }

    template <typename T> Command &withOptionArgument(std::shared_ptr<OptionArgument<T>> arg)
    {
        safeAddToArgGroup(arg);
        optionArguments.push_back(arg);
        return *this;
    }

    template <typename T> Command &withOptionArgument(OptionArgument<T> &&arg)
    {
        return withOptionArgument(std::make_shared<OptionArgument<T>>(std::move(arg)));
    }

    Command &withFlagArgument(std::shared_ptr<FlagArgument> arg)
    {
        safeAddToArgGroup(arg);
        flagArguments.push_back(arg);
        return *this;
    }

    Command &withFlagArgument(FlagArgument &&arg)
    {
        return withFlagArgument(std::make_shared<FlagArgument>(std::move(arg)));
    }

    Command &withExecutionFunc(std::unique_ptr<std::function<void(const CliContext &)>> actionPtr);

    Command &withExecutionFunc(std::function<void(const CliContext &)> &&action)
    {
        return withExecutionFunc(
            std::make_unique<std::function<void(const CliContext &)>>(std::move(action)));
    }

    Command &withSubCommand(std::unique_ptr<Command> subCommandPtr);

    Command &withSubCommand(Command &&subCommand)
    {
        return withSubCommand(std::make_unique<Command>(std::move(subCommand)));
    }

    template <typename... Args>
        requires((sizeof...(Args) > 0) &&
                 (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
    Command &withExclusiveGroup(Args &&...args);

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

// end --- command.h --- 



namespace cli::commands
{
class CommandNotFoundException : public std::runtime_error
{
public:
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

class CommandTree
{
public:
    explicit CommandTree(std::string_view rootName);

    // Insert as child under a chain of parent ids
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

    void insert(std::unique_ptr<Command> cmd) // insert at root
    {
        root->withSubCommand(std::move(cmd));
    }

    // Lookup via identifier chain
    template <typename... Ids> Command *find(Ids &&...ids) const
    {
        return findRecursive(root.get(), std::forward<Ids>(ids)...);
    }

    // Iterate over all commands in the tree (DFS)
    void forEachCommand(const std::function<void(Command *)> &func) const
    {
        if (root)
        {
            forEachCommandRecursive(root.get(), func);
        }
    }

    void forEachCommand(const std::function<void(Command &)> &func) const
    {
        if (root)
        {
            forEachCommandRecursive(root.get(), func);
        }
    }

    // Print tree
    void print(std::ostream &os, int indent = 0) const;

    Command *getRootCommand() { return root.get(); }

    const Command *getRootCommand() const { return root.get(); }

    std::string_view getPathForCommand(Command *cmd) const;
    void buildCommandPathMap(const std::string &separator = " ");

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
};
} // namespace cli::commands


// end --- command_tree.h --- 



// begin --- docwriting.h --- 

#pragma once

namespace cli::commands::docwriting
{

class DocsNotBuildException : public std::runtime_error
{
public:
    using runtime_error::runtime_error;
};

class DocWriter
{

public:
    explicit DocWriter(const CliConfig &config) : configuration(config) {}

    void setDocStrings(Command &command, std::string_view fullCommandPath) const;

    std::string generateLongDocString(const Command &command,
                                      std::string_view fullCommandPath) const;

    std::string generateShortDocString(const Command &command,
                                       std::string_view fullCommandPath) const;

    std::string generateOptionsDocString(const FlagArgument &argument) const;

    std::string generateArgDocString(const FlagArgument &argument) const;

    std::string generateOptionsDocString(const OptionArgumentBase &argument) const;

    std::string generateArgDocString(const OptionArgumentBase &argument) const;

    std::string generateOptionsDocString(const PositionalArgumentBase &argument) const;

    std::string generateArgDocString(const PositionalArgumentBase &argument) const;

private:
    void addGroupArgumentDocString(std::ostringstream &builder,
                                   const cli::commands::ArgumentGroup &groupArgs) const;
    const CliConfig &configuration;
};

} // namespace cli::commands::docwriting

// end --- docwriting.h --- 



// begin --- parser.h --- 

#pragma once
#include <memory>
#include <string>
#include <vector>

namespace cli::parsing
{
class StringParser
{
public:
    explicit StringParser(const CliConfig &config) : configuration(config) {}

    void parseArguments(const cli::commands::Command &command,
                        const std::vector<std::string> &inputs,
                        ContextBuilder &contextBuilder) const;

private:
    std::any parseRepeatableList(const cli::commands::TypedArgumentBase &arg,
                                 const std::string &input) const;

    void parseRepeatable(const cli::commands::OptionArgumentBase &arg, const std::string &input,
                         ContextBuilder &ContextBuilder) const;

    void parseRepeatable(const cli::commands::PositionalArgumentBase &arg, const std::string &input,
                         ContextBuilder &ContextBuilder) const;

    void checkGroups(const cli::commands::Command &command,
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

// end --- parser.h --- 



#define RUN_CLI_APP(cliInstance, argc_, argv_)                                                     \
    try                                                                                            \
    {                                                                                              \
        return cliInstance.run(argc_, argv_);                                                      \
    }                                                                                              \
    catch (const std::exception &e)                                                                \
    {                                                                                              \
        cliInstance.Logger().error()                                                               \
            << "terminate called after throwing an instance of '" << typeid(e).name() << "'\n"     \
            << "  what(): " << e.what() << std::endl;                                              \
        std::abort();                                                                              \
    }

namespace cli
{
class CliBase
{
public:
    // Non-copyable
    CliBase(const CliBase &) = delete;
    CliBase &operator=(const CliBase &) = delete;

    CliBase(CliBase &&) = default;
    CliBase &operator=(CliBase &&) = default;

    explicit CliBase(std::string_view executableName);
    explicit CliBase(CliConfig &&config);
    ~CliBase() = default;

    commands::Command &createNewCommand(
        std::string_view id, std::unique_ptr<std::function<void(const CliContext &)>> actionPtr);

    commands::Command &createNewCommand(std::string_view id)
    {
        return createNewCommand(id, nullptr);
    };

    CliBase &withCommand(std::unique_ptr<commands::Command> subCommandPtr);

    CliBase &withCommand(commands::Command &&subCommand)
    {
        return withCommand(std::make_unique<commands::Command>(std::move(subCommand)));
    }

    [[nodiscard]] const commands::CommandTree &getCommandTree() const { return commandsTree; };

    [[nodiscard]] CliConfig &getConfig() { return *configuration; };

    void init();
    int run(int argc, char *argv[]);

    [[nodiscard]] logging::Logger &Logger() { return *logger; }

    void setLogger(std::unique_ptr<logging::Logger> &newLogger) { logger = std::move(newLogger); }

    void setLogger(std::unique_ptr<logging::Logger> &&newLogger) { logger = std::move(newLogger); }

private:
    int internalRun(std::span<char *const> args) const;
    const commands::Command *locateCommand(std::vector<std::string> &args) const;
    bool rootShortCircuits(std::vector<std::string> &args, const cli::commands::Command &cmd) const;
    bool commandShortCircuits(std::vector<std::string> &args,
                              const cli::commands::Command &cmd) const;
    void globalHelp() const;

    commands::CommandTree commandsTree;
    bool initialized{false};

    std::unique_ptr<logging::Logger> logger =
        std::make_unique<logging::Logger>(logging::LogLevel::DEBUG);

    std::unique_ptr<CliConfig> configuration;
    parsing::StringParser parser;
    cli::commands::docwriting::DocWriter docWriter;
};
} // namespace cli

// end --- cli_base.h --- 



#include <iostream>

namespace cli
{
CliBase::CliBase(CliConfig &&config)
    : commandsTree(config.executableName),
      configuration(std::make_unique<CliConfig>(std::move(config))), parser(*configuration),
      docWriter(*configuration)
{
}

CliBase::CliBase(std::string_view executableName)
    : commandsTree(executableName), configuration(std::make_unique<CliConfig>()),
      parser(*configuration), docWriter(*configuration)
{
    configuration->executableName = std::string(executableName);
}

commands::Command &CliBase::createNewCommand(
    std::string_view id, std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
{
    auto cmd = std::make_unique<commands::Command>(id, "", "",
                                                   std::move(actionPtr)); // default-constructed
    commands::Command &ref = *cmd;
    commandsTree.insert(std::move(cmd));
    return ref;
}

CliBase &CliBase::withCommand(std::unique_ptr<commands::Command> subCommandPtr)
{
    commandsTree.insert(std::move(subCommandPtr));
    return *this;
}

void CliBase::init()
{
    initialized = true;

    commandsTree.buildCommandPathMap();

    commandsTree.forEachCommand([this](commands::Command *cmd) {
        docWriter.setDocStrings(*cmd, commandsTree.getPathForCommand(cmd));
    });
}

// NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays)
int CliBase::run(int argc, char *argv[])
{
    if (!initialized)
    {
        init();
    }
    return internalRun(std::span<char *const>(argv + 1, argc - 1));
}

int CliBase::internalRun(std::span<char *const> args) const
{
    std::vector<std::string> argVec(args.begin(), args.end());

    if (rootShortCircuits(argVec, *(commandsTree.getRootCommand())))
    {
        return 0;
    }

    if (const auto *cmd = locateCommand(argVec))
    {
        if (commandShortCircuits(argVec, *cmd))
        {
            return 0;
        }

        logger->trace("Executing command: {}", cmd->getIdentifier());

        auto contextBuilder = cli::ContextBuilder();

        parser.parseArguments(*cmd, argVec, contextBuilder);
        cmd->execute(*contextBuilder.build(*logger));
    }
    else
    {
        std::cout << "Unknown command: " << args[0] << "\n";
        globalHelp();
    }
    return 0;
}

// returns the found command and modifies args to only contain the values that
// werent consumed in the tree traversal
const commands::Command *CliBase::locateCommand(std::vector<std::string> &args) const
{
    const commands::Command *commandPtr = commandsTree.getRootCommand();

    int consumed = 0;

    for (const auto &arg : args)
    {
        // Move one level down if child exists
        const auto *subCommandPtr = commandPtr->getSubCommand(arg);
        if (!subCommandPtr)
        {
            break;
        }

        commandPtr = subCommandPtr;
        ++consumed;
    }
    args.erase(args.begin(), args.begin() + consumed);
    return commandPtr;
}

bool CliBase::rootShortCircuits(std::vector<std::string> &args,
                                const cli::commands::Command &cmd) const
{
    if (args.empty() && !cmd.hasExecutionFunction())
    {
        globalHelp();
        return true;
    }

    if (args.size() == 1)
    {
        if (args.at(0) == "-h" || args.at(0) == "--help")
        {
            globalHelp();
            return true;
        }
        else if (args.at(0) == "-v" || args.at(0) == "--version")
        {
            logger->info("Version: {}", configuration->version);
            return true;
        }
    }
    return false;
}

bool CliBase::commandShortCircuits(std::vector<std::string> &args,
                                   const cli::commands::Command &cmd) const
{
    if (args.size() == 1 && (args.at(0) == "-h" || args.at(0) == "--help"))
    {
        logger->info(std::string(cmd.getDocStringLong()));
        return true;
    }
    return false;
}

void CliBase::globalHelp() const
{
    logger->info() << configuration->description << "\n\n";

    auto printCmd = [this](const commands::Command *cmd) {
        if (cmd->hasExecutionFunction())
            logger->info() << cmd->getDocStringShort() << "\n\n";
    };

    commandsTree.forEachCommand(printCmd);

    logger->info() << "Use --help <command> to get more information about a specific command"
                   << std::endl;
}
} // namespace cli

// end --- cli_base.cpp --- 



// begin --- cli_context.cpp --- 



#include <sstream>

namespace cli
{

bool CliContext::isOptionArgPresent(const std::string &argName) const
{
    return optionArgs->contains(argName);
}

bool CliContext::isPositionalArgPresent(const std::string &argName) const
{
    return positionalArgs->contains(argName);
}

bool CliContext::isFlagPresent(const std::string &argName) const
{
    return flagArgs->contains(argName);
}

bool CliContext::isArgPresent(const std::string &argName) const
{
    return isOptionArgPresent(argName) || isFlagPresent(argName) || isPositionalArgPresent(argName);
}

ContextBuilder::ContextBuilder()
{
    positionalArgs = std::make_unique<std::unordered_map<std::string, std::any>>();
    optionalArgs = std::make_unique<std::unordered_map<std::string, std::any>>();
    flagArgs = std::make_unique<std::unordered_set<std::string>>();
}

ContextBuilder &ContextBuilder::addPositionalArgument(const std::string &argName, std::any &val)
{
    positionalArgs->try_emplace(argName, val);
    return *this;
}

ContextBuilder &ContextBuilder::addPositionalArgument(std::string_view argName, std::any &val)
{
    positionalArgs->try_emplace(std::string(argName), val);
    return *this;
}

ContextBuilder &ContextBuilder::addOptionArgument(const std::string &argName, std::any &val)
{
    optionalArgs->try_emplace(argName, val);
    return *this;
}

ContextBuilder &ContextBuilder::addOptionArgument(std::string_view argName, std::any &val)
{
    optionalArgs->try_emplace(std::string(argName), val);
    return *this;
}

ContextBuilder &ContextBuilder::addFlagArgument(const std::string &argName)
{
    flagArgs->insert(argName);
    return *this;
}

ContextBuilder &ContextBuilder::addFlagArgument(std::string_view argName)
{
    flagArgs->insert(std::string(argName));
    return *this;
}

bool ContextBuilder::isArgPresent(const std::string &argName) const
{
    return optionalArgs->contains(argName) || flagArgs->contains(argName) ||
           positionalArgs->contains(argName);
}

std::unique_ptr<CliContext> ContextBuilder::build(cli::logging::Logger &logger)
{
    return std::make_unique<CliContext>(std::move(positionalArgs), std::move(optionalArgs),
                                        std::move(flagArgs), logger);
}

std::string MissingArgumentException::makeMessage(
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

std::string InvalidArgumentTypeException::makeMessage(const std::string &name,
                                                      const std::type_info &requested,
                                                      const std::type_info &actual)
{
    std::ostringstream oss;
    oss << "Invalid type for argument: \"" << name << "\"\n"
        << "Requested type: " << requested.name() << "\n"
        << "Actual type: " << actual.name();
    return oss.str();
}
} // namespace cli

// end --- cli_context.cpp --- 



// begin --- argument_group.cpp --- 



#include <algorithm>

namespace cli::commands
{
bool ArgumentGroup::isRequired() const
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

// end --- argument_group.cpp --- 



// begin --- command.cpp --- 



#include <format>
#include <iostream>

namespace cli::commands
{
std::string_view Command::getDocStringShort() const
{
    if (docStringShort.empty())
    {
        throw docwriting::DocsNotBuildException(
            std::format("Short documentation string not built for command '{}'.", identifier));
    }
    return docStringShort;
}

std::string_view Command::getDocStringLong() const
{
    if (docStringLong.empty())
    {
        throw docwriting::DocsNotBuildException(
            std::format("Long documentation string not built for command '{}'.", identifier));
    }
    return docStringLong;
}

void Command::execute(const CliContext &context) const
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

Command &Command::withShortDescription(std::string_view desc)
{
    shortDescription = desc;
    return *this;
}

Command &Command::withLongDescription(std::string_view desc)
{
    longDescription = desc;
    return *this;
}

Command &Command::withExecutionFunc(
    std::unique_ptr<std::function<void(const CliContext &)>> actionPtr)
{
    executePtr = std::move(actionPtr);
    return *this;
}

Command &Command::withSubCommand(std::unique_ptr<Command> subCommandPtr)
{
    subCommands.try_emplace(subCommandPtr->identifier, std::move(subCommandPtr));
    return *this;
}

void Command::safeAddToArgGroup(const std::shared_ptr<ArgumentBase> &arg)
{
    if (indexForNewArgGroup >= argumentGroups.size())
    {
        argumentGroups.push_back(std::make_unique<ArgumentGroup>(false, false));
    }
    argumentGroups[indexForNewArgGroup]->addArgument(arg);
}

void Command::addArgGroup(const ArgumentGroup &argGroup)
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

Command *Command::getSubCommand(std::string_view id)
{
    auto it = subCommands.find(id);
    return (it != subCommands.end()) ? it->second.get() : nullptr;
}

const Command *Command::getSubCommand(std::string_view id) const
{
    auto it = subCommands.find(id);
    return (it != subCommands.end()) ? it->second.get() : nullptr;
}

std::ostream &operator<<(std::ostream &out, const Command &cmd)
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

std::string MalformedCommandException::buildMessage(const Command &cmd, const std::string &msg)
{
    std::ostringstream oss;
    oss << "Malformed Command: " << cmd.getIdentifier();
    if (!msg.empty())
        oss << " - " << msg;
    return oss.str();
}

} // namespace cli::commands


// end --- command.cpp --- 



// begin --- command_tree.cpp --- 



#include <sstream>

namespace cli::commands
{
CommandTree::CommandTree(std::string_view rootName)
{
    root = std::make_unique<Command>(rootName);
}

std::string_view CommandTree::getPathForCommand(Command *cmd) const
{
    return commandPathMap.at(cmd);
}

void CommandTree::buildCommandPathMap(const std::string &separator)
{
    std::unordered_map<Command *, std::string> map;
    if (root)
    {
        std::vector<std::string> path;
        buildCommandPathMapRecursive(root.get(), path, separator);
    }
}

void CommandTree::buildCommandPathMapRecursive(Command *cmd, std::vector<std::string> &path,
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

std::string CommandNotFoundException::buildMessage(const std::string &id,
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
} // namespace cli::commands

// end --- command_tree.cpp --- 



// begin --- docwriting.cpp --- 


#include <format>
#include <iomanip>
#include <sstream>
#include <string>

#define inline_t

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

void DocWriter::addGroupArgumentDocString(std::ostringstream &builder,
                                          const cli::commands::ArgumentGroup &groupArgs) const
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
        builder << argPtr->getArgDocString(*this);

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

std::string DocWriter::generateShortDocString(const Command &command,
                                              std::string_view fullCommandPath) const
{
    std::ostringstream builder;
    builder << fullCommandPath << " ";

    for (const auto &argGroupPtr : command.getArgumentGroups())
    {
        addGroupArgumentDocString(builder, *argGroupPtr);
        builder << " ";
    }
    builder << "\n" << command.getShortDescription();
    return builder.str();
}

void DocWriter::setDocStrings(Command &command, std::string_view fullCommandPath) const
{
    command.docStringLong = generateLongDocString(command, fullCommandPath);
    command.docStringShort = generateShortDocString(command, fullCommandPath);
}

std::string DocWriter::generateLongDocString(const Command &command,
                                             std::string_view fullCommandPath) const
{
    std::ostringstream builder;
    builder << fullCommandPath << " ";

    for (const auto &argGroupPtr : command.getArgumentGroups())
    {
        addGroupArgumentDocString(builder, *argGroupPtr);
        builder << ' ';
    }

    builder << "\n\n" << command.getLongDescription() << "\n\nOptions:\n";

    for (const auto &argGroupPtr : command.getArgumentGroups())
    {
        for (const auto &argPtr : argGroupPtr->getArguments())
        {
            builder << argPtr->getOptionsDocString(*this) << "\n";
        }
    }
    return builder.str();
}

std::string DocWriter::generateOptionsDocString(const FlagArgument &argument) const
{
    std::ostringstream builder;
    builder << argument.getName() << ' ' << argument.getShortName();
    return std::format("{:<{}}{:>{}}", builder.str(), configuration.optionsWidth,
                       argument.getUsageComment(), argument.getUsageComment().size());
}

std::string DocWriter::generateArgDocString(const FlagArgument &argument) const
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getOptionArgumentBrackets(argument.isRequired());
    builder << inBracket << argument.getName() << ',' << argument.getShortName() << outBracket;
    return builder.str();
}

std::string DocWriter::generateOptionsDocString(const OptionArgumentBase &argument) const
{
    std::ostringstream builder;
    builder << argument.getName() << ',' << argument.getShortName() << ' ' << '<'
            << argument.getValueName() << '>';
    if (argument.isRepeatable())
        builder << "...";
    return std::format("{:<{}}{:>{}}", builder.str(), configuration.optionsWidth,
                       argument.getUsageComment(), argument.getUsageComment().size());
}

std::string DocWriter::generateArgDocString(const OptionArgumentBase &argument) const
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getOptionArgumentBrackets(argument.isRequired());
    builder << inBracket << argument.getName() << ' ' << argument.getShortName() << ' ';
    builder << '<' << argument.getValueName() << '>' << outBracket;

    if (argument.isRepeatable())
        builder << "...";
    return builder.str();
}

std::string DocWriter::generateOptionsDocString(const PositionalArgumentBase &argument) const
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getPositionalArgumentBrackets(argument.isRequired());

    builder << inBracket << argument.getName() << outBracket;
    if (argument.isRepeatable())
        builder << "...";
    return std::format("{:<{}}{:>{}}", builder.str(), configuration.optionsWidth,
                       argument.getUsageComment(), argument.getUsageComment().size());
}

std::string DocWriter::generateArgDocString(const PositionalArgumentBase &argument) const
{
    std::ostringstream builder;
    auto [inBracket, outBracket] = getPositionalArgumentBrackets(argument.isRequired());

    builder << inBracket << argument.getName();
    builder << outBracket;

    if (argument.isRepeatable())
        builder << "...";
    return builder.str();
}
} // namespace cli::commands::docwriting

// end --- docwriting.cpp --- 



// begin --- flag_argument.cpp --- 



namespace cli::commands
{
std::string FlagArgument::getOptionsDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateOptionsDocString(*this);
}

std::string FlagArgument::getArgDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateArgDocString(*this);
}

} // namespace cli::commands

// end --- flag_argument.cpp --- 



// begin --- option_argument.cpp --- 



namespace cli::commands
{
std::string OptionArgumentBase::getOptionsDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateOptionsDocString(*this);
}

std::string OptionArgumentBase::getArgDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateArgDocString(*this);
}
} // namespace cli::commands

// end --- option_argument.cpp --- 



// begin --- positional_argument.cpp --- 



namespace cli::commands
{
std::string PositionalArgumentBase::getOptionsDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateOptionsDocString(*this);
}

std::string PositionalArgumentBase::getArgDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateArgDocString(*this);
}
} // namespace cli::commands

// end --- positional_argument.cpp --- 



// begin --- formatter.cpp --- 



#include <chrono>
#include <iomanip>
#include <sstream>

namespace cli::logging
{

std::string BasicFormatter::format(const LogRecord &record) const
{
    auto const time = timezone->to_local(record.timestamp);

    std::ostringstream oss;
    oss << "[" << std::format("{:%Y-%m-%d %X}", time) << "] " << toString(record.level) << ": "
        << record.message << "\n";
    return oss.str();
}

std::string MessageOnlyFormatter::format(const LogRecord &record) const
{
    return record.message + "\n";
}

} // namespace cli::logging

// end --- formatter.cpp --- 



// begin --- handler.cpp --- 



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

// end --- handler.cpp --- 



// begin --- logger.cpp --- 



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

void Logger::addHandler(std::unique_ptr<IHandler> handlerPtr)
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

// end --- logger.cpp --- 



// begin --- parser.cpp --- 



#include <algorithm>
#include <any>
#include <iostream>
#include <ranges>

// used by heady
#define inline_t

namespace cli::parsing
{
std::any StringParser::parseRepeatableList(const cli::commands::TypedArgumentBase &arg,
                                           const std::string &input) const
{
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

void StringParser::parseRepeatable(const cli::commands::OptionArgumentBase &arg,
                                   const std::string &input, ContextBuilder &contextBuilder) const
{
    auto values = parseRepeatableList(arg, input);
    contextBuilder.addOptionArgument(arg.getShortName(), values);
    contextBuilder.addOptionArgument(arg.getName(), values);
}

void StringParser::parseRepeatable(const cli::commands::PositionalArgumentBase &arg,
                                   const std::string &input, ContextBuilder &contextBuilder) const
{
    auto values = parseRepeatableList(arg, input);
    contextBuilder.addPositionalArgument(arg.getName(), values);
}

bool StringParser::tryOptionArg(
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
                throw ParseException("Non Repeatable Argument was repeated");
            }

            auto val = matchedOpt->parseToValue(inputs[index + 1]);
            contextBuilder.addOptionArgument(matchedOpt->getName(), val);
            contextBuilder.addOptionArgument(matchedOpt->getShortName(), val);
        }
        index++;
        return true;
    }
    return false;
}

bool StringParser::tryFlagArg(
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

void cli::parsing::StringParser::parseArguments(const cli::commands::Command &command,
                                                const std::vector<std::string> &inputs,
                                                ContextBuilder &contextBuilder) const
{
    const auto &posArguments = command.getPositionalArguments();
    const auto &optArguments = command.getOptionArguments();
    const auto &flagArguments = command.getFlagArguments();

    size_t posArgsIndex = 0;
    for (size_t i = 0; i < inputs.size(); ++i)
    {
        const auto &input = inputs[i];
        if (tryOptionArg(optArguments, inputs, input, i, contextBuilder))
        {
            i++;
            continue;
        }

        if (tryFlagArg(flagArguments, input, contextBuilder))
        {
            continue;
        }

        if (posArgsIndex >= posArguments.size())
        {
            throw ParseException("More positional arguments provided than command accepts");
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
                throw ParseException("Non Repeatable Argument was repeated");
            }
            auto val = posArg.parseToValue(input);
            contextBuilder.addPositionalArgument(posArg.getName(), val);
        }

        ++posArgsIndex;
    }
    checkGroups(command, contextBuilder);
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
        else if (contextBuilder.isArgPresent(std::string(argPtr->getName())))
        {
            throw GroupParseException("Two arguments of mutually exclusive group were present");
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
            throw GroupParseException("Not all arguments of mutually inclusive group were present");
        }
    }
}

void StringParser::checkGroups(const cli::commands::Command &command,
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
    }
}
} // namespace cli::parsing


// end --- parser.cpp --- 



// begin --- utils.h --- 

#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace cli
{

std::vector<std::string> turnArgsToVector(int argc, char *argv[]);

void printVector(const std::vector<std::string> &vec, std::ostream &os);

} // namespace cli

// end --- utils.h --- 

