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

struct GreenDragonComponent {};
struct RedDragonComponent {};

struct ElfWarriorComponent {
  int health = 5;
  float speed = 250.f;
};
