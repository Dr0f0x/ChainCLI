#include "positional_argument.h"
#include "docwriting/docwriting.h"

namespace cli::commands
{
std::string PositionalArgumentBase::getOptionsDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateOptionsDocString(*this);
}

std::string PositionalArgumentBase::getArgDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateArgDocString(*this);
}
} // namespace cli::commands