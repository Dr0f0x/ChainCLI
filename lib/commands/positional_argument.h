#pragma once
#include <ostream>
#include <string>
#include <any>
#include <typeindex>
#include <string_view>
#include "argument.h"
#include "parsing/parser_utils.h"

namespace cli::commands
{
    class PositionalArgumentBase : public ArgumentBase
    {
    public:
        // Movable
        PositionalArgumentBase(PositionalArgumentBase &&) noexcept = default;
        PositionalArgumentBase &operator=(PositionalArgumentBase &&) noexcept = default;

        [[nodiscard]] std::type_index getType() const { return type; }
        [[nodiscard]] virtual std::any parseToValue(const std::string &input) const = 0;

    protected:
        PositionalArgumentBase(std::string_view name,
                               std::string_view usage_comment,
                               bool required,
                               std::type_index t)
            : ArgumentBase(name, usage_comment, required), type(t) {}

        std::type_index type;
    };

    template <typename T>
    class PositionalArgument : public PositionalArgumentBase
    {
        friend std::ostream &operator<<(std::ostream &out, const PositionalArgument<T> &arg)
        {
            out << arg.name << " (" << arg.usageComment << ")";
            return out;
        }

    public:
        explicit PositionalArgument(std::string_view name,
                                    std::string_view usage_comment = "",
                                    bool required = true)
            : PositionalArgumentBase(name, usage_comment, required, typeid(T)) {}

        // Movable
        PositionalArgument(PositionalArgument &&) noexcept = default;
        PositionalArgument &operator=(PositionalArgument &&) noexcept = default;

        [[nodiscard]] std::any parseToValue(const std::string &input) const override;

        std::string getOptionsDocString() const override;
        std::string getArgDocString() const override;

        PositionalArgument<T> &withOptionsComment(std::string_view usage_comment)
        {
            usageComment = usage_comment;
            return *this;
        }

        PositionalArgument<T> &withRequired(bool req)
        {
            required = req;
            return *this;
        }
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
}