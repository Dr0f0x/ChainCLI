#include "positional_argument.h"
#include "docwriting/docwriting.h"

namespace cli::commands
{
    std::string PositionalArgumentBase::getOptionsDocString() const
    {
        return docwriting::generateOptionsDocString(*this);
    }

    std::string PositionalArgumentBase::getArgDocString() const
    {
        return docwriting::generateArgDocString(*this);
    }
} // namespace cli::commands