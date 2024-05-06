#include "utils.hpp"

#include <fstream>

namespace utils {

auto read_file(const std::filesystem::path &path) -> std::string {
  auto fs = std::ifstream{path.c_str()};
  auto ss = std::stringstream{};
  ss << fs.rdbuf();
  return ss.str();
}

} // namespace utils
