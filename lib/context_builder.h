#pragma once
#include <any>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

#include "cli_context.h"
#include "logging/logger.h"

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
    std::unique_ptr<CliContext> build(cli::logging::Logger &logger);

private:
    std::unique_ptr<std::unordered_map<std::string, std::any>> positionalArgs;
    std::unique_ptr<std::unordered_map<std::string, std::any>> optionalArgs;
    std::unique_ptr<std::unordered_set<std::string>> flagArgs;
};

} // namespace cli