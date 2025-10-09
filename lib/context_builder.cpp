#include "context_builder.h"

namespace cli
{
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
} // namespace cli