#pragma once
#include <ostream>
#include <functional>
#include <string_view>

namespace cli::commands {
    void test();

    class Command
    {
        friend std::ostream& operator<<(std:: ostream& out, const Command& person);
    public:
        // Constructor initializes the identifier and description
        constexpr Command(std::string_view id, std::string_view desc, std::function<void()> action) 
            : identifier(id), description(desc), executeFunc(action) {}

        // Accessors
        [[nodiscard]] constexpr std::string_view getIdentifier() const noexcept { return identifier; }
        [[nodiscard]] constexpr std::string_view getDescription() const noexcept { return description; }

        // Virtual destructor for base class
        virtual ~Command() = default;

        //try to run the passed callable
        void execute() const;

    private:
        const std::string_view identifier;
        const std::string_view description;
        std::function<void()> executeFunc;
    };
} // namespace cli::commands