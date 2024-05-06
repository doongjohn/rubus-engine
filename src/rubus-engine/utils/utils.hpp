#pragma once

#include <string>
#include <filesystem>

namespace utils {

auto read_file(const std::filesystem::path &path) -> std::string;

} // namespace utils
