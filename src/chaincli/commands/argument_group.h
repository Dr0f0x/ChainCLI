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
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

#include "argument.h"

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
