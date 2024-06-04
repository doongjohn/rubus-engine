#pragma once

#include <rubus-ecs/ecs.hpp>
#include <rubus-engine/game/game.hpp>

struct TransformComponent {
  glm::vec3 position = {0, 0, 0};
};

struct SpriteComponent {
  rugame::Sprite *sprite = nullptr;

  inline ~SpriteComponent() {
    delete sprite;
  }
};

struct CharacterComponent {
  std::string name;
  int health = 1;
  int ap = 1;
};
