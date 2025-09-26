#include "formatter.h"
#include <sstream>
#include <iomanip>
#include <ctime>

namespace cli::logging
{

    std::string BasicFormatter::format(const LogRecord &record) const
    {
        std::time_t t = std::chrono::system_clock::to_time_t(record.timestamp);
        std::tm tm = *std::localtime(&t);

        std::ostringstream oss;
        oss << "[" << std::put_time(&tm, "%F %T") << "] "
            << toString(record.level) << ": " << record.message;
        return oss.str();
    }

    std::string MessageOnlyFormatter::format(const LogRecord &record) const
    {
        return record.message;
    }

} // namespace cli::logging