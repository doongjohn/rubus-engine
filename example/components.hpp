#pragma once

#include <rubus-ecs/ecs.hpp>
#include <rubus-engine/game/game.hpp>

struct TransformComponent {
  glm::vec3 position = glm::vec3{0, 0, 0};

  static auto deinit(std::span<uint8_t>) -> void {}
};

struct SpriteComponent {
  rugame::Sprite *sprite = nullptr;

  static auto deinit(std::span<uint8_t> value) -> void {
    auto component = reinterpret_cast<SpriteComponent *>(value.data());
    delete component->sprite;
  }
};

struct GreenDragonComponent {
  static auto deinit(std::span<uint8_t>) -> void {}
};

struct RedDragonComponent {
  static auto deinit(std::span<uint8_t>) -> void {}
};

struct ElfWarriorComponent {
  int health = 5;
  float speed = 250.f;

  static auto deinit(std::span<uint8_t>) -> void {}
};
