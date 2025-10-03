#pragma once
#include <vector>
#include <memory>
#include <type_traits>
#include <utility>
#include "argument.h"

namespace cli::commands
{
    class ArgumentGroup
    {
    public:
        template <typename... Args>
            requires((sizeof...(Args) > 0) &&
                     (!std::same_as<std::remove_cvref_t<Args>, ArgumentGroup> && ...) &&
                     (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
        explicit ArgumentGroup(bool exclusive, bool inclusive, Args &&...args)
            : exclusive(exclusive), inclusive(inclusive)
        {
            (arguments.emplace_back(
                 std::make_shared<std::remove_cvref_t<Args>>(std::forward<Args>(args))),
             ...);
        }

        [[nodiscard]] const std::vector<std::shared_ptr<ArgumentBase>> &getArguments() const noexcept
        {
            return arguments;
        }
        [[nodiscard]] bool isExclusive() const noexcept { return exclusive; }
        [[nodiscard]] bool isInclusive() const noexcept { return inclusive; }

    private:
        std::vector<std::shared_ptr<ArgumentBase>> arguments;
        bool exclusive;
        bool inclusive;
    };

    class ExclusiveGroup : public ArgumentGroup
    {
    public:
        template <typename... Args>
            requires((sizeof...(Args) > 0) &&
                     (!std::same_as<std::remove_cvref_t<Args>, ExclusiveGroup> && ...) &&
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
            requires((sizeof...(Args) > 0) &&
                     (!std::same_as<std::remove_cvref_t<Args>, InclusiveGroup> && ...) &&
                     (std::derived_from<std::remove_cvref_t<Args>, ArgumentBase> && ...))
        explicit InclusiveGroup(Args &&...args)
            : ArgumentGroup(false, true, std::forward<Args>(args)...)
        {
        }
    };
} // namespace cli::commands
