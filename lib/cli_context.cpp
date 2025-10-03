#include "cli_context.h"
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
        return optionalArgs->contains(argName) || flagArgs->contains(argName) || positionalArgs->contains(argName);
    }

    std::unique_ptr<CliContext> ContextBuilder::build(cli::logging::Logger &logger)
    {
        return std::make_unique<CliContext>(std::move(positionalArgs), std::move(optionalArgs), std::move(flagArgs), logger);
    }

    std::string MissingArgumentException::makeMessage(const std::string &name, const std::unordered_map<std::string, std::any> &args)
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