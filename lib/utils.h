#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace cli
{

std::vector<std::string> turnArgsToVector(int argc, char *argv[]);

void printVector(const std::vector<std::string> &vec, std::ostream &os);

} // namespace cli