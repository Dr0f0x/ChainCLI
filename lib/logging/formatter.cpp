#include "formatter.h"

#include <chrono>
#include <iomanip>
#include <sstream>

namespace cli::logging
{

std::string BasicFormatter::format(const LogRecord &record) const
{
    auto const time = timezone->to_local(record.timestamp);

    std::ostringstream oss;
    oss << "[" << std::format("{:%Y-%m-%d %X}", time) << "] " << toString(record.level) << ": "
        << record.message << "\n";
    return oss.str();
}

std::string MessageOnlyFormatter::format(const LogRecord &record) const
{
    return record.message + "\n";
}

} // namespace cli::logging