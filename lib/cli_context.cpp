#include "cli_context.h"
#include <sstream>

namespace cli
{

    bool CliContext::isOptionArgPresent(const std::string &argName) const
    {
        return optArgsPresence->contains(argName);
    }

    ContextBuilder::ContextBuilder()
    {
        positionalArgs = std::make_unique<std::map<std::string, std::any, std::less<>>>();
        optArgsPresence = std::make_unique<std::unordered_set<std::string>>();
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

    ContextBuilder &ContextBuilder::addOptionArgumentPresence(const std::string &argName)
    {
        optArgsPresence->insert(argName);
        return *this;
    }

    ContextBuilder &ContextBuilder::addOptionArgumentPresence(std::string_view argName)
    {
        optArgsPresence->insert(std::string(argName));
        return *this;
    }

    std::unique_ptr<CliContext> ContextBuilder::build()
    {
        return std::make_unique<CliContext>(std::move(positionalArgs), std::move(optArgsPresence));
    }

    std::string MissingArgumentException::makeMessage(const std::string &name, const std::map<std::string, std::any, std::less<>> &args)
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

    std::string InvalidArgumentTypeException::makeMessage(const std::string &name, const std::type_info &requested, const std::type_info &actual)
    {
        std::ostringstream oss;
        oss << "Invalid type for argument: \"" << name << "\"\n"
            << "Requested type: " << requested.name() << "\n"
            << "Actual type: " << actual.name();
        return oss.str();
    }
} // namespace cli