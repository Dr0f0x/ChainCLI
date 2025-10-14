#include "context_exception.h"
#include <sstream>

namespace cli
{
    std::string MissingArgumentException::makeMessage(
    const std::string &name, const std::unordered_map<std::string, std::any> &args)
{
    std::ostringstream oss;
    oss << "Missing argument: \"" << name << "\" was not passed in this context.\n";
    oss << "Available arguments: ";
    if (args.empty())
    {
        oss << "<none>";
    }
    else
    {
        bool first = true;
        for (auto &[k, _] : args)
        {
            if (!first)
                oss << ", ";
            oss << k;
            first = false;
        }
    }
    return oss.str();
}

std::string InvalidArgumentTypeException::makeMessage(const std::string &name,
                                                      const std::type_info &requested,
                                                      const std::type_info &actual)
{
    std::ostringstream oss;
    oss << "Invalid type for argument: \"" << name << "\"\n"
        << "Requested type: " << requested.name() << "\n"
        << "Actual type: " << actual.name();
    return oss.str();
}
} //namespace cli