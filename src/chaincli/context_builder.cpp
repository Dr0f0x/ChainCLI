// Copyright 2025 Dominik Czekai
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "context_builder.h"

#ifdef CHAIN_CLI_VERBOSE
#include <iostream>
#endif

#define inline_t

namespace cli
{
inline_t ContextBuilder::ContextBuilder()
    : positionalArgs(std::make_unique<std::unordered_map<std::string, std::any>>()),
      optionalArgs(std::make_unique<std::unordered_map<std::string, std::any>>()),
      flagArgs(std::make_unique<std::unordered_set<std::string>>())
{
}

inline_t ContextBuilder &ContextBuilder::addPositionalArgument(const std::string &argName, std::any &val)
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Appending positional argument: " << argName << "\n";
#endif
    positionalArgs->try_emplace(argName, val);
    return *this;
}

inline_t ContextBuilder &ContextBuilder::addPositionalArgument(std::string_view argName, std::any &val)
{
    return addPositionalArgument(std::string(argName), val);
}

inline_t ContextBuilder &ContextBuilder::addRepeatablePositionalArgument(const std::string &argName, const std::vector<std::any> &values)
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

inline_t ContextBuilder &ContextBuilder::addRepeatablePositionalArgument(std::string_view argName, const std::vector<std::any> &values)
{
    return addRepeatablePositionalArgument(std::string(argName), values);
}

inline_t ContextBuilder &ContextBuilder::addOptionArgument(const std::string &argName, std::any &val)
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Adding option argument: " << argName << "\n";
#endif
    optionalArgs->try_emplace(argName, val);
    return *this;
}

inline_t ContextBuilder &ContextBuilder::addOptionArgument(std::string_view argName, std::any &val)
{
    return addOptionArgument(std::string(argName), val);
}

inline_t ContextBuilder &ContextBuilder::addRepeatableOptionArgument(const std::string &argName, const std::vector<std::any> &values)
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

inline_t ContextBuilder &ContextBuilder::addRepeatableOptionArgument(std::string_view argName, const std::vector<std::any> &values)
{
    return addRepeatableOptionArgument(std::string(argName), values);
}

inline_t ContextBuilder &ContextBuilder::addFlagArgument(const std::string &argName)
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Adding flag argument: " << argName << "\n";
#endif
    flagArgs->insert(argName);
    return *this;
}

inline_t ContextBuilder &ContextBuilder::addFlagArgument(std::string_view argName)
{
    flagArgs->insert(std::string(argName));
    return *this;
}

inline_t bool ContextBuilder::isArgPresent(const std::string &argName) const
{
    return optionalArgs->contains(argName) || flagArgs->contains(argName) ||
           positionalArgs->contains(argName);
}

inline_t std::unique_ptr<CliContext> ContextBuilder::build(cli::logging::AbstractLogger &logger)
{
#ifdef CHAIN_CLI_VERBOSE
    std::cout << "Building CliContext with " << positionalArgs->size() << " positional, " 
              << optionalArgs->size() << " option, and " << flagArgs->size() << " flag arguments\n";
#endif
    return std::make_unique<CliContext>(std::move(positionalArgs), std::move(optionalArgs),
                                        std::move(flagArgs), logger);
}
} // namespace cli