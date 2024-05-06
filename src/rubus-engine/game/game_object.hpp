#pragma once

#include <rubus-engine/app/app.hpp>

namespace rugame {

struct Scene;

struct GameObject {
  virtual ~GameObject() = default;

  virtual auto update(ruapp::Window *window, Scene *scene, double delta) -> void;
};

} // namespace game
