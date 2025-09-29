#pragma once
#include <ostream>
#include <string>
#include <typeindex>
#include <typeinfo>

namespace cli::commands
{
    class PositionalArgument
    {
        friend std::ostream &operator<<(std::ostream &out, const PositionalArgument &arg);

    public:
        constexpr PositionalArgument(std::string_view name, std::type_index t, std::string_view short_name, const std::string_view &usage_comment, bool required)
            : name(name), shortName(short_name), usageComment(usage_comment), required(required), type(t) {}
        explicit constexpr PositionalArgument(std::string_view name, std::type_index t)
            : name(name), shortName(""), usageComment(""), type(t) {}

        // Movable
        PositionalArgument(PositionalArgument &&) noexcept = default;
        PositionalArgument &operator=(PositionalArgument &&) noexcept = default;

        [[nodiscard]] constexpr std::string_view getName() const noexcept { return name; }
        [[nodiscard]] constexpr std::string_view getShortName() const noexcept { return shortName; }
        [[nodiscard]] constexpr std::string_view getUsageComment() const noexcept { return usageComment; }
        [[nodiscard]] constexpr bool isRequired() const noexcept { return required; }
        [[nodiscard]] std::type_index getType() const { return type; }

        virtual ~PositionalArgument() = default;

        bool hasShortName() const { return !shortName.empty(); }

        // documentation to be used under Options for command
        [[nodiscard]] std::string getOptionsDocString() const;

        // documentation to be used in the command display (example command display: run <--help> [args] etc)
        [[nodiscard]] std::string getArgDocString() const;

        PositionalArgument &withShortName(std::string_view short_name);
        PositionalArgument &withUsageComment(std::string_view usage_comment);
        PositionalArgument &withRequired(bool req);

    private:
        const std::string name;
        std::string shortName;
        std::string usageComment;
        bool required{true};

        std::type_index type;
    };

    // Factory function
    template <typename T>
    PositionalArgument newArgument(std::string_view name, std::string_view short_name = "", std::string_view usage_comment = "", bool required = true)
    {
        PositionalArgument arg(name, typeid(T), short_name, usage_comment, required);
        return arg;  // RVO/move elision instead of copies?
    }
} // namespace cli::commands