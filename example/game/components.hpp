#pragma once

#include <rubus-ecs/ecs.hpp>
#include <rubus-engine/game/game.hpp>

#include "data.hpp"

struct TransformComponent {
  glm::vec3 position = {0, 0, 0};
};

struct SpriteComponent {
  rugame::Sprite *sprite = nullptr;

  inline ~SpriteComponent() {
    delete sprite;
  }
};

struct MonsterComponent {
  std::string name;
  int health = 0;
  int damage = 0;

  glm::vec3 start_pos = {0, 0, 0};
  float end_time = 0;
  float time = 0;
  bool action_done = false;

  inline MonsterComponent(int health, int damage, glm::vec3 start_pos)
      : health{health}, damage{damage}, start_pos{start_pos} {}
};

struct CharacterComponent {
  std::string name;
  int health = 0;
  int ap = 0;
  std::array<SkillData, 3> skills;

  inline CharacterComponent(CharacterData data) {
    name = data.name;
    health = data.health;
    ap = data.ap;
    skills = data.skills;
  }
};

struct SkillComponent {
  SkillData data;
  float end_time = 0.5f;
  float time = 0;
};
