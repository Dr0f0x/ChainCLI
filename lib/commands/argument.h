#pragma once
#include <ostream>
#include <string>
#include <any>
#include <typeindex>
#include <string_view>
#include "parsing/parser_utils.h"

namespace cli::commands
{
    // TODO really this should also be a templated type
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
        [[nodiscard]] std::type_index getType() const { return type; }

        [[nodiscard]] virtual std::string getOptionsDocString() const = 0;
        [[nodiscard]] virtual std::string getArgDocString() const = 0;

        [[nodiscard]] virtual std::any parseToValue(const std::string &input) const = 0;

    protected:
        ArgumentBase(std::string_view name,
                     std::string_view usage_comment,
                     bool required,
                     std::type_index t)
            : name(name), usageComment(usage_comment), required(required), type(t) {}

        const std::string name;
        std::string usageComment;
        bool required{true};
        std::type_index type;
    };

    template <typename T>
    class PositionalArgument : public ArgumentBase
    {
        friend std::ostream &operator<<(std::ostream &out, const PositionalArgument<T> &arg)
        {
            out << arg.name << " (" << arg.usageComment << ")";
            return out;
        }

    public:
        explicit PositionalArgument(std::string_view name,
                                    std::string_view short_name = "",
                                    std::string_view usage_comment = "",
                                    bool required = true)
            : ArgumentBase(name, usage_comment, required, typeid(T)),
              shortName(short_name) {}

        // Movable
        PositionalArgument(PositionalArgument &&) noexcept = default;
        PositionalArgument &operator=(PositionalArgument &&) noexcept = default;

        [[nodiscard]] constexpr std::string_view getShortName() const noexcept { return shortName; }

        [[nodiscard]] std::any parseToValue(const std::string &input) const override;

        std::string getOptionsDocString() const override;
        std::string getArgDocString() const override;

        PositionalArgument<T> &withShortName(std::string_view short_name)
        {
            shortName = short_name;
            return *this;
        }

        PositionalArgument<T> &withUsageComment(std::string_view usage_comment)
        {
            usageComment = usage_comment;
            return *this;
        }

        PositionalArgument<T> &withRequired(bool req)
        {
            required = req;
            return *this;
        }

    private:
        std::string shortName;
    };

    template <typename T>
    inline std::any PositionalArgument<T>::parseToValue(const std::string &input) const
    {
        return cli::parsing::ParseHelper::parse<T>(input);
    }

    template <typename T>
    inline std::string PositionalArgument<T>::getOptionsDocString() const
    {
        return "Options doc string for " + std::string(name);
    }

    template <typename T>
    inline std::string PositionalArgument<T>::getArgDocString() const
    {
        return "Argument doc string for " + std::string(name);
    }

} // namespace cli::commands