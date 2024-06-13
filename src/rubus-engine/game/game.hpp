#pragma once

#include <string>

#include <glm/glm.hpp>

#include <rubus-engine/graphics/graphics.hpp>

namespace rugame {

struct Screen {
  float width = 0;
  float height = 0;
};

struct Camera2d {
  Screen *screen;
  glm::mat4 projection;
  glm::mat4 view;

  glm::vec3 position;

  Camera2d() = default;
  Camera2d(Screen *screen, glm::vec3 position);

  auto update() -> void;
  auto world_to_screen_space(glm::mat4 world_transform) -> glm::vec2;
  auto screen_to_world_space(glm::vec2 screen_pos) -> glm::vec2;
};

struct SpriteMaterial {
  inline static uint32_t shader = 0;
  std::string texture = "";

  SpriteMaterial() = default;
  SpriteMaterial(std::string texture);

  static auto init() -> void;
  static auto deinit() -> void;

  auto bind() -> void;
  auto unbind() -> void;
};

struct Sprite {
  glm::mat4 transform = glm::mat4{1.f};
  int32_t zorder = 0;

  graphics::Mesh mesh;
  SpriteMaterial material;

  Sprite() = default;
  Sprite(glm::vec2 pivot, float width, float height, SpriteMaterial material);
  ~Sprite();

  auto draw(Camera2d *camera) -> void;
};

} // namespace rugame
