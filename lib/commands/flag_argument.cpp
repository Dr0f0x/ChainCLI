#include "flag_argument.h"
#include "docwriting/docwriting.h"

namespace cli::commands
{
std::string FlagArgument::getOptionsDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateOptionsDocString(*this);
}

std::string FlagArgument::getArgDocString(const docwriting::DocWriter &writer) const
{
    return writer.generateArgDocString(*this);
}

} // namespace cli::commands