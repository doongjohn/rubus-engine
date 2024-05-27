#pragma once

#include "glm/ext/matrix_transform.hpp"

#include <rubus-engine/game/scene.hpp>
#include <rubus-engine/game/resource.hpp>
#include "../components.hpp"

inline auto new_game_scene() -> rugame::Scene * {
  auto scene = new rugame::Scene{};

  scene->fn_on_init = [](rugame::Scene *) {
    rugame::ResourceManager::load_texture2d_pixel("character.elf_warrior", "assets/character/elf_warrior.png");
    rugame::ResourceManager::load_texture2d_pixel("monster.green_dragon", "assets/monster/green_dragon.png");
    rugame::ResourceManager::load_texture2d_pixel("monster.red_dragon", "assets/monster/red_dragon.png");
    rugame::SpriteMaterial::init();
  };

  scene->fn_on_deinit = [](rugame::Scene *) {
    rugame::ResourceManager::unload_texture2d();
    rugame::SpriteMaterial::deinit();
  };

  scene->fn_on_start = [](ruapp::Window *, rugame::SceneManager *, rugame::Scene *scene) {
    std::srand(std::time(nullptr));

    // spawn GreenDragon
    for (auto i = 0; i < 8; ++i) {
      auto position = glm::vec3{rand() % 600 - 300, rand() % 600 - 300, 0.f};
      auto sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"monster.green_dragon"}};
      auto e = scene->arch_storage.create_entity();
      e.add_component<TransformComponent>(position);
      e.add_component<SpriteComponent>(sprite);
      e.add_component<GreenDragonComponent>();
    }

    // spawn RedDragon
    for (auto i = 0; i < 8; ++i) {
      auto position = glm::vec3{rand() % 600 - 300, rand() % 600 - 300, 0.f};
      auto sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"monster.red_dragon"}};
      auto e = scene->arch_storage.create_entity();
      e.add_component<TransformComponent>(position);
      e.add_component<SpriteComponent>(sprite);
      e.add_component<RedDragonComponent>();
    }

    // spawn Player
    {
      auto sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"character.elf_warrior"}};
      auto e = scene->arch_storage.create_entity();
      e.add_component<TransformComponent>();
      e.add_component<SpriteComponent>(sprite);
      e.add_component<PlayerComponent>();
    }

    // init ui
    scene->ui_node_hashmap.insert(
      {"player",
       (new rugui::Node{"player", "health: "})->set_font_size(30)->set_flex_self_align(rugui::FlexAlign::Center)});

    scene->ui_tree.root
      ->add(
        (new rugui::Node{"title", "Example game"})->set_font_size(50)->set_flex_self_align(rugui::FlexAlign::Center))
      ->add(scene->ui_node_hashmap.at("player"));
  };

  scene->fn_on_update = [](ruapp::Window *window, rugame::SceneManager *scene_manager, rugame::Scene *scene, double) {
    if (window->is_key_just_down(VK_ESCAPE)) {
      scene_manager->set_active_scene("main_menu");
    }

    if (window->is_key_just_down(VK_RETURN)) {
      // spawn Player
      auto sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"character.elf_warrior"}};
      auto e = scene->arch_storage.create_entity();
      e.add_component<TransformComponent>();
      e.add_component<SpriteComponent>(sprite);
      e.add_component<PlayerComponent>();
    }

    static auto query_player = ruecs::Query{}.with<TransformComponent, PlayerComponent>();
    static auto query_green_dragon = ruecs::Query{}.with<TransformComponent, GreenDragonComponent>();
    static auto query_red_dragon = ruecs::Query{}.with<TransformComponent, RedDragonComponent>();
    static auto query_render = ruecs::Query{}.with<TransformComponent, SpriteComponent>();

    // update player
    for_each_entities(&scene->arch_storage, &scene->command, query_player) {
      auto transform = entity.get_component<TransformComponent>();
      auto player = entity.get_component<PlayerComponent>();

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

      const auto movement = input_dir * (float)(player->speed * scene->delta);
      transform->position += glm::vec3{movement, 0.f};
    }

    // handle collision
    for_each_entities(&scene->arch_storage, &scene->command, query_player) {
      auto player_entity = entity;
      auto player_transform = entity.get_component<TransformComponent>();
      auto player = entity.get_component<PlayerComponent>();

      // green dragon
      for_each_entities(&scene->arch_storage, &scene->command, query_green_dragon) {
        auto dragon_transform = entity.get_component<TransformComponent>();
        if (glm::distance(dragon_transform->position, player_transform->position) < 50.f) {
          scene->command.delete_entity(entity);
          player->health += 1;
        }
      }

      // red dragon
      for_each_entities(&scene->arch_storage, &scene->command, query_red_dragon) {
        auto dragon_transform = entity.get_component<TransformComponent>();
        if (glm::distance(dragon_transform->position, player_transform->position) < 50.f) {
          scene->command.delete_entity(entity);
          player->health -= 1;
          if (player->health == 0) {
            scene->command.delete_entity(player_entity);
          }
        }
      }
    }

    // update ui
    for_each_entities(&scene->arch_storage, &scene->command, query_player) {
      auto player = entity.get_component<PlayerComponent>();
      scene->ui_node_hashmap.at("player")->text = std::format("health: {}", player->health);
    }

    scene->command.run();

    // update render list
    for_each_entities(&scene->arch_storage, &scene->command, query_render) {
      auto transform = entity.get_component<TransformComponent>();
      auto sprite_component = entity.get_component<SpriteComponent>();
      auto sprite = sprite_component->sprite;
      sprite->transform = glm::translate(glm::mat4{1}, transform->position);
      scene->layers[0].push_back(sprite);
    }
  };

  return scene;
}
