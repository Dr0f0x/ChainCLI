#pragma once
#include <map>
#include <string>
#include <any>
#include <memory>
#include <stdexcept>
#include <functional>
#include <unordered_set>

namespace cli
{
    class MissingArgumentException : public std::runtime_error
    {
    public:
        MissingArgumentException(const std::string &name,
                                 const std::map<std::string, std::any, std::less<>> &args)
            : std::runtime_error(makeMessage(name, args)) {}

    private:
        static std::string makeMessage(const std::string &name,
                                       const std::map<std::string, std::any, std::less<>> &args);
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
        explicit CliContext(std::unique_ptr<std::map<std::string, std::any, std::less<>>> posArgs,
        std::unique_ptr<std::unordered_set<std::string>> optArgsPresence)
            : positionalArgs(std::move(posArgs)), optArgsPresence(std::move(optArgsPresence)) {}

        // Non-copyable
        CliContext(const CliContext &) = delete;
        CliContext &operator=(const CliContext &) = delete;

        bool isOptionArgPresent(const std::string &argName) const;

        template <typename T>
        T getPositionalArgument(const std::string &argName) const
        {
            return getAnyCast<T>(argName);
        }

        template <typename T>
        void getPositionalArgument(const std::string &argName, T &out) const
        {
            out = getAnyCast<T>(argName);
        }

    private:
        std::unique_ptr<std::map<std::string, std::any, std::less<>>> positionalArgs;
        std::unique_ptr<std::unordered_set<std::string>> optArgsPresence;

        template <typename T>
        T getAnyCast(const std::string &name) const
        {
            try
            {
                auto it = positionalArgs->find(name);
                if (it == positionalArgs->end())
                {
                    throw MissingArgumentException(name, *positionalArgs);
                }
                return std::any_cast<T>(it->second);
            }
            catch (const std::bad_any_cast &)
            {
                throw InvalidArgumentTypeException(name, typeid(T), positionalArgs->at(name).type());
            }
        }
    };

    class ContextBuilder
    {
    public:
        ContextBuilder();

        ContextBuilder &addPositionalArgument(const std::string &argName, std::any &val);
        ContextBuilder &addPositionalArgument(std::string_view argName, std::any &val);
        ContextBuilder &addOptionArgumentPresence(const std::string &argName);
        ContextBuilder &addOptionArgumentPresence(std::string_view argName);

        std::unique_ptr<CliContext> build();

    private:
        std::unique_ptr<std::map<std::string, std::any, std::less<>>> positionalArgs;
        std::unique_ptr<std::unordered_set<std::string>> optArgsPresence;
    };
}