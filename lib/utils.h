#pragma once
#include <string>
#include <vector>
#include <string_view>

namespace cli{

    std::vector<std::string> turnArgsToVector(int argc, char* argv[]);

} // namespace cli