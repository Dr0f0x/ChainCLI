#pragma once
#include <string>
#include <any>
#include <typeindex>
#include <string_view>

namespace cli::commands
{
    enum class ArgumentKind
    {
        Positional,
        Option,
        Flag,
    };

    class ArgumentBase
    {
    public:
        virtual ~ArgumentBase() = default;

        // Movable
        ArgumentBase(ArgumentBase &&) noexcept = default;
        ArgumentBase &operator=(ArgumentBase &&) noexcept = default;

        [[nodiscard]] constexpr std::string_view getName() const noexcept { return name; }
        [[nodiscard]] constexpr std::string_view getUsageComment() const noexcept { return optionsComment; }
        [[nodiscard]] constexpr bool isRequired() const noexcept { return required; }
        [[nodiscard]] constexpr bool isRepeatable() const noexcept { return repeatable; }
        [[nodiscard]] constexpr ArgumentKind getArgType() const { return argType; }

        [[nodiscard]] virtual std::string getOptionsDocString() const = 0;
        [[nodiscard]] virtual std::string getArgDocString() const = 0;

    protected:
        ArgumentBase(std::string_view name,
                     std::string_view optionsComment,
                     ArgumentKind argType,
                     bool repeatable,
                     bool required)
            : name(name), optionsComment(optionsComment), argType(argType), repeatable(repeatable), required(required) {}

        std::string name;
        std::string optionsComment;
        ArgumentKind argType;
        bool repeatable{false};
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
                          std::string_view optionsComment,
                          ArgumentKind argType,
                          bool repeatable,
                          bool required,
                          std::type_index t)
            : ArgumentBase(name, optionsComment, argType, repeatable, required), type(t) {}

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