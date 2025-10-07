#pragma once
#include <any>
#include <string>
#include <string_view>
#include <typeindex>

namespace cli::commands
{
enum class ArgumentKind
{
    Positional,
    Option,
    Flag,
};

namespace docwriting
{
class DocWriter; // Forward declaration
}

class ArgumentBase
{
public:
    virtual ~ArgumentBase() = default;

    // Movable
    ArgumentBase(ArgumentBase &&) noexcept = default;
    ArgumentBase &operator=(ArgumentBase &&) noexcept = default;

    // Copyable
    ArgumentBase(const ArgumentBase &) = default;
    ArgumentBase &operator=(const ArgumentBase &) = default;

    [[nodiscard]] constexpr std::string_view getName() const noexcept { return name; }

    [[nodiscard]] constexpr std::string_view getUsageComment() const noexcept
    {
        return optionsComment;
    }

    [[nodiscard]] constexpr bool isRequired() const noexcept { return required; }

    [[nodiscard]] constexpr bool isRepeatable() const noexcept { return repeatable; }

    [[nodiscard]] constexpr ArgumentKind getArgType() const { return argType; }

    [[nodiscard]] virtual std::string getOptionsDocString(
        const docwriting::DocWriter &writer) const = 0;
    [[nodiscard]] virtual std::string getArgDocString(
        const docwriting::DocWriter &writer) const = 0;

protected:
    ArgumentBase(std::string_view name, std::string_view optionsComment, ArgumentKind argType,
                 bool repeatable, bool required)
        : name(name), optionsComment(optionsComment), argType(argType), repeatable(repeatable),
          required(required)
    {
    }

    const std::string name;
    std::string optionsComment;
    const ArgumentKind argType;
    bool repeatable{false};
    bool required{true};
};

class TypedArgumentBase
{
public:
    virtual ~TypedArgumentBase() = default;

    // Copyable
    TypedArgumentBase(const TypedArgumentBase &) = default;
    TypedArgumentBase &operator=(const TypedArgumentBase &) = default;

    // Movable
    TypedArgumentBase(TypedArgumentBase &&) noexcept = default;
    TypedArgumentBase &operator=(TypedArgumentBase &&) noexcept = default;

    [[nodiscard]] std::type_index getType() const { return type; }

    [[nodiscard]] virtual std::any parseToValue(const std::string &input) const = 0;

protected:
    explicit TypedArgumentBase(std::type_index t) : type(t) {}

    const std::type_index type;
};

class FlaggedArgumentBase
{
public:
    [[nodiscard]] constexpr std::string_view getShortName() const noexcept { return shortName; }

protected:
    explicit FlaggedArgumentBase(std::string_view shortName) : shortName(shortName) {}

    std::string shortName;
};
} // namespace cli::commands