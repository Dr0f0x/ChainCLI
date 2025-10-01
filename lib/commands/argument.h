#pragma once
#include <string>
#include <any>
#include <typeindex>
#include <string_view>

namespace cli::commands
{
    class ArgumentBase
    {
    public:
        virtual ~ArgumentBase() = default;

        // Movable
        ArgumentBase(ArgumentBase &&) noexcept = default;
        ArgumentBase &operator=(ArgumentBase &&) noexcept = default;

        [[nodiscard]] constexpr std::string_view getName() const noexcept { return name; }
        [[nodiscard]] constexpr std::string_view getUsageComment() const noexcept { return usageComment; }
        [[nodiscard]] constexpr bool isRequired() const noexcept { return required; }

        [[nodiscard]] virtual std::string getOptionsDocString() const = 0;
        [[nodiscard]] virtual std::string getArgDocString() const = 0;

    protected:
        ArgumentBase(std::string_view name,
                     std::string_view usage_comment,
                     bool required)
            : name(name), usageComment(usage_comment), required(required) {}

        const std::string name;
        std::string usageComment;
        bool required{true};
    };
} // namespace cli::commands