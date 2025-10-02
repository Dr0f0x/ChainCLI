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

    class TypedArgumentBase : public ArgumentBase
    {
    public:
        // Movable
        TypedArgumentBase(TypedArgumentBase &&) noexcept = default;
        TypedArgumentBase &operator=(TypedArgumentBase &&) noexcept = default;

        [[nodiscard]] std::type_index getType() const { return type; }
        [[nodiscard]] virtual std::any parseToValue(const std::string &input) const = 0;

    protected:
        TypedArgumentBase(std::string_view name,
                          std::string_view usage_comment,
                          bool required,
                          std::type_index t)
            : ArgumentBase(name, usage_comment, required), type(t) {}

        std::type_index type;
    };

    class FlaggedArgumentBase
    {
    public:
        [[nodiscard]] constexpr std::string_view getShortName() const noexcept { return shortName; }

    protected:
        explicit FlaggedArgumentBase(std::string_view shortName)
            : shortName(shortName) {}
        std::string shortName;
    };
} // namespace cli::commands