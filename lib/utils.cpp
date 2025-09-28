#include "utils.h"
#include <sstream>

namespace cli{

std::vector<std::string> turnArgsToVector(int argc, char* argv[]) {
    if (argc <= 1) return {};              // no arguments besides program name
    return std::vector<std::string>(argv + 1, argv + argc); // skip argv[0]
}
} // namespace cli