#pragma once

#include <rubus-engine/game/scene.hpp>
#include <rubus-engine/game/game_object.hpp>

struct GreenDragon : public rugame::GameObject {
  rugame::Sprite *sprite = nullptr;

  GreenDragon() {
    sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"monster.green_dragon"}};
    position = glm::vec3{rand() % 600 - 300, rand() % 600 - 300, 0.f};
    sprite->transform = glm::translate(sprite->transform, position);
  }
  ~GreenDragon() override {
    delete sprite;
  }

  inline auto update(ruapp::Window *, rugame::Scene *scene, double) -> void override {
    scene->render_list.push_back(sprite);
  }
};

struct RedDragon : public rugame::GameObject {
  rugame::Sprite *sprite = nullptr;

  RedDragon() {
    sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"monster.red_dragon"}};
    position = glm::vec3{rand() % 600 - 300, rand() % 600 - 300, 0.f};
    sprite->transform = glm::translate(sprite->transform, position);
  }
  ~RedDragon() override {
    delete sprite;
  }

  inline auto update(ruapp::Window *, rugame::Scene *scene, double) -> void override {
    scene->render_list.push_back(sprite);
  }
};

struct ElfWarrior : public rugame::GameObject {
  rugame::Sprite *sprite = nullptr;
  int health = 5;
  float speed = 250.f;

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
    const auto movement = input_dir * (speed * delta);
    position += glm::vec3{movement, 0.f};

    sprite->transform = glm::mat4{1.f};
    sprite->transform = glm::translate(sprite->transform, position);

    for (auto game_object : scene->game_objects) {
      if (game_object != this) {
        if (glm::distance(game_object->position, position) < 40) {
          if (dynamic_cast<GreenDragon *>(game_object)) {
            health += 1;
          }
          if (dynamic_cast<RedDragon *>(game_object)) {
            health -= 1;
            if (health == 0) {
              scene->destroy_game_object(this);
            }
          }
          scene->destroy_game_object(game_object);
        }
      }
    }
    scene->ui_node_hashmap.at("player")->text = std::format("health: {}", health);

    scene->render_list.push_back(sprite);
  }
};
