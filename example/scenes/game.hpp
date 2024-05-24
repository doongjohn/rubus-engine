#pragma once

#include "glm/ext/matrix_transform.hpp"

#include <rubus-engine/game/scene.hpp>
#include <rubus-engine/game/resource.hpp>
#include "../components.hpp"

inline auto new_game_scene(rugame::SceneManager *scene_manager) -> rugame::Scene * {
  auto scene = new rugame::Scene{};

  scene->fn_init = [](rugame::Scene *) {
    rugame::ResourceManager::load_texture2d_pixel("character.elf_warrior", "assets/character/elf_warrior.png");
    rugame::ResourceManager::load_texture2d_pixel("monster.green_dragon", "assets/monster/green_dragon.png");
    rugame::ResourceManager::load_texture2d_pixel("monster.red_dragon", "assets/monster/red_dragon.png");
    rugame::SpriteMaterial::init();
  };

  scene->fn_deinit = [](rugame::Scene *) {
    rugame::ResourceManager::unload_texture2d();
    rugame::SpriteMaterial::deinit();
  };

  scene->fn_start = [&](ruapp::Window *, rugame::Scene *scene) {
    std::srand(std::time(nullptr));

    // init game objects
    for (auto i = 0; i < 10; ++i) {
      auto position = glm::vec3{rand() % 600 - 300, rand() % 600 - 300, 0.f};
      auto sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"monster.green_dragon"}};
      auto e = scene->arch_storage.new_entity();
      scene->arch_storage.add_component<TransformComponent>(e, position);
      scene->arch_storage.add_component<SpriteComponent>(e, sprite);
      scene->arch_storage.add_component<GreenDragonComponent>(e);
    }
    for (auto i = 0; i < 10; ++i) {
      auto position = glm::vec3{rand() % 600 - 300, rand() % 600 - 300, 0.f};
      auto sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"monster.red_dragon"}};
      auto e = scene->arch_storage.new_entity();
      scene->arch_storage.add_component<TransformComponent>(e, position);
      scene->arch_storage.add_component<SpriteComponent>(e, sprite);
      scene->arch_storage.add_component<RedDragonComponent>(e);
    }
    {
      auto sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"character.elf_warrior"}};
      auto e = scene->arch_storage.new_entity();
      scene->arch_storage.add_component<TransformComponent>(e);
      scene->arch_storage.add_component<SpriteComponent>(e, sprite);
      scene->arch_storage.add_component<ElfWarriorComponent>(e);
    }

    // init ui
    scene->ui_node_hashmap.insert(
      {"player",
       (new rugui::Node{"player", "health: "})->set_font_size(30)->set_flex_self_align(rugui::FlexAlign::Start)});

    scene->ui_tree.root
      ->add(
        (new rugui::Node{"title", "Example game"})->set_font_size(50)->set_flex_self_align(rugui::FlexAlign::Center))
      ->add(scene->ui_node_hashmap.at("player"));
  };

  scene->fn_update = [=](ruapp::Window *window, rugame::Scene *, double) {
    if (window->is_key_just_down(VK_ESCAPE)) {
      scene_manager->set_active_scene("main_menu");
    }

    static auto query_render = ruecs::Query{}.with<TransformComponent, SpriteComponent>();
    static auto query_player = ruecs::Query{}.with<TransformComponent, ElfWarriorComponent>();

    for_each_entities(&scene->arch_storage, query_render) {
      auto transform = arch->get_component<TransformComponent>(entity);
      auto sprite_component = arch->get_component<SpriteComponent>(entity);
      auto sprite = sprite_component->sprite;
      sprite->transform = glm::translate(glm::mat4{1}, transform->position);
      scene->render_list.push_back(sprite);
    }

    for_each_entities(&scene->arch_storage, query_player) {
      auto transform = arch->get_component<TransformComponent>(entity);
      auto elf_warrior = arch->get_component<ElfWarriorComponent>(entity);

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
      const auto movement = input_dir * (float)(elf_warrior->speed * scene->delta_time);
      transform->position += glm::vec3{movement, 0.f};

      scene->ui_node_hashmap.at("player")->text = std::format("health: {}", elf_warrior->health);
    }
  };

  return scene;
}
