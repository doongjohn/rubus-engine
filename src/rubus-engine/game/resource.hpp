#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

namespace rugame {

struct TextureResource {
  uint32_t handle = 0;
  int width = 0;
  int height = 0;
};

struct ResourceManager {
  inline static std::unordered_map<std::string, TextureResource> texture2d;

  static auto load_texture2d_pixel(const std::string &key, const char *file_path) -> void;
  static auto unload_texture2d(const std::string &key) -> void;
  static auto unload_texture2d() -> void;
};

} // namespace game
