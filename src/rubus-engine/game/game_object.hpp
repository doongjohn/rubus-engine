#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <rubus-engine/app/app.hpp>

namespace rugame {

struct Scene;

struct GameObject {
  std::size_t index = 0;
  glm::vec3 position = glm::vec3{0, 0, 0};

  virtual ~GameObject() = default;

  virtual auto update(ruapp::Window *window, Scene *scene, double delta) -> void;
};

} // namespace rugame
