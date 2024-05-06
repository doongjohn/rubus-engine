#include "resource.hpp"

#include <format>
#include <iostream>

#include <stb_image.h>
#include <glad/glad.h>

namespace rugame {

auto ResourceManager::load_texture2d_pixel(const std::string &key, const char *file_path) -> void {
  if (texture2d.contains(key)) {
    std::cerr << std::format("Error: texture key \"{}\" already exists\n", key);
    return;
  }

  auto texture = uint32_t{};
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  float borderColor[] = {0.0f, 0.0f, 0.0f, 0.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  auto width = 0;
  auto height = 0;
  auto channels = 0;
  stbi_set_flip_vertically_on_load(true);
  auto data = stbi_load(file_path, &width, &height, &channels, STBI_rgb_alpha);
  if (data != nullptr) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    std::cerr << std::format("Error: failed to load texture file \"{}\"\n", file_path);
    return;
  }
  stbi_image_free(data);

  texture2d.insert({key, TextureResource{.handle = texture, .width = width, .height = height}});
}

auto ResourceManager::unload_texture2d(const std::string &key) -> void {
  if (texture2d.contains(key)) {
    auto texture_res = ResourceManager::texture2d.at(key);
    glDeleteTextures(1, &texture_res.handle);
    texture2d.erase(key);
  }
}

auto ResourceManager::unload_texture2d() -> void {
  for (const auto &[key, texture_res] : texture2d) {
    glDeleteTextures(1, &texture_res.handle);
  }
  texture2d.clear();
}

} // namespace game
