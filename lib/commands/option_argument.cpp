#include "option_argument.h"
#include "docwriting/docwriting.h"

namespace cli::commands
{
    std::string OptionArgumentBase::getOptionsDocString() const
{
    return docwriting::generateOptionsDocString(*this);
}

std::string OptionArgumentBase::getArgDocString() const
{
    return docwriting::generateArgDocString(*this);
}
} // namespace cli::commands