#include "option_argument.h"
#include "docwriting/docwriting.h"

namespace cli::commands
{
std::string OptionArgumentBase::getOptionsDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateOptionsDocString(*this);
}

std::string OptionArgumentBase::getArgDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateArgDocString(*this);
}
} // namespace cli::commands