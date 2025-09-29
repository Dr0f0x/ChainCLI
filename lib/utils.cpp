#include "utils.h"
#include <sstream>

namespace cli
{

    std::vector<std::string> turnArgsToVector(int argc, char *argv[])
    {
        if (argc <= 1)
            return {};                                          // no arguments besides program name
        return std::vector<std::string>(argv + 1, argv + argc); // skip argv[0]
    }

    void printVector(const std::vector<std::string> &vec, std::ostream& os)
    {
        for (const auto &item : vec)
        {
            os << item << "\n";
        }
    }
} // namespace cli