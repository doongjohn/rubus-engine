#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <rubus-engine/game/scene.hpp>
#include <rubus-engine/game/game_object.hpp>

struct Dragon : public rugame::GameObject {
  rugame::Sprite *sprite = nullptr;

  Dragon() {
    sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"monster.green_dragon"}};
    sprite->transform = glm::translate(sprite->transform, glm::vec3{rand() % 600 - 300, rand() % 600 - 300, 0.f});
  }
  ~Dragon() override {
    delete sprite;
  }

  inline auto update(ruapp::Window *, rugame::Scene *scene, double) -> void override {
    scene->render_queue.push_back(sprite);
  }
};

struct ElfWarrior : public rugame::GameObject {
  rugame::Sprite *sprite = nullptr;

  ElfWarrior() {
    sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"character.elf_warrior"}};
  }
  ~ElfWarrior() override {
    delete sprite;
  }

  inline auto update(ruapp::Window *window, rugame::Scene *scene, double delta) -> void override {
    auto input_dir = glm::vec2{0.f, 0.f};
    if (window->is_key_down(VK_RIGHT)) {
      input_dir.x = +1;
    }
    if (window->is_key_down(VK_LEFT)) {
      input_dir.x = -1;
    }
    if (window->is_key_down(VK_UP)) {
      input_dir.y = +1;
    }
    if (window->is_key_down(VK_DOWN)) {
      input_dir.y = -1;
    }
    if (glm::length(input_dir) > 0) {
      input_dir = glm::normalize(input_dir);
    }
    const auto movement = input_dir * (120.f * delta);
    sprite->transform = glm::translate(sprite->transform, glm::vec3{movement, 0.f});

    scene->render_queue.push_back(sprite);
  }
};
