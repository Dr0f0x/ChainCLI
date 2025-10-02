#pragma once
#include <map>
#include <string>
#include <any>
#include <memory>
#include <stdexcept>
#include <functional>
#include <unordered_set>
#include "logging/logger.h"

namespace cli
{
    class MissingArgumentException : public std::runtime_error
    {
    public:
        MissingArgumentException(const std::string &name,
                                 const std::unordered_map<std::string, std::any> &args)
            : std::runtime_error(makeMessage(name, args)) {}

    private:
        static std::string makeMessage(const std::string &name,
                                       const std::unordered_map<std::string, std::any> &args);
    };

    class InvalidArgumentTypeException : public std::runtime_error
    {
    public:
        InvalidArgumentTypeException(const std::string &name,
                                     const std::type_info &requested,
                                     const std::type_info &actual)
            : std::runtime_error(makeMessage(name, requested, actual)) {}

    private:
        static std::string makeMessage(const std::string &name,
                                       const std::type_info &requested,
                                       const std::type_info &actual);
    };

    class CliContext
    {
    public:
        explicit CliContext(
            std::unique_ptr<std::unordered_map<std::string, std::any>> posArgs,
            std::unique_ptr<std::unordered_map<std::string, std::any>> optArgs,
            std::unique_ptr<std::unordered_set<std::string>> flagArgs,
            cli::logging::Logger& logger)
            : positionalArgs(std::move(posArgs)), optionArgs(std::move(optArgs)), flagArgs(std::move(flagArgs)), Logger(logger) {}

        // Non-copyable
        CliContext(const CliContext &) = delete;
        CliContext &operator=(const CliContext &) = delete;

        bool isOptionArgPresent(const std::string &argName) const;
        bool isPositionalArgPresent(const std::string &argName) const;
        bool isFlagPresent(const std::string &argName) const;

        template <typename T>
        T getPositionalArgument(const std::string &argName) const
        {
            return getAnyCast<T>(argName, *positionalArgs);
        }

        template <typename T>
        void getPositionalArgument(const std::string &argName, T &out) const
        {
            out = getAnyCast<T>(argName, *positionalArgs);
        }

        template <typename T>
        T getOptionArgument(const std::string &argName) const
        {
            return getAnyCast<T>(argName, *optionArgs);
        }

        template <typename T>
        void getOptionArgument(const std::string &argName, T &out) const
        {
            out = getAnyCast<T>(argName, *optionArgs);
        }

        cli::logging::Logger& Logger;

    private:
        std::unique_ptr<std::unordered_map<std::string, std::any>> positionalArgs;
        std::unique_ptr<std::unordered_map<std::string, std::any>> optionArgs;
        std::unique_ptr<std::unordered_set<std::string>> flagArgs;

        template <typename T>
        static T getAnyCast(const std::string &name, std::unordered_map<std::string, std::any>& dict)
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

        std::unique_ptr<CliContext> build(cli::logging::Logger& logger);

    private:
        std::unique_ptr<std::unordered_map<std::string, std::any>> positionalArgs;
        std::unique_ptr<std::unordered_map<std::string, std::any>> optionalArgs;
        std::unique_ptr<std::unordered_set<std::string>> flagArgs;
    };
}