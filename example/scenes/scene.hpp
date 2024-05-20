#pragma once

#include "glm/ext/matrix_transform.hpp"

#include <rubus-engine/game/scene.hpp>
#include <rubus-engine/game/resource.hpp>
#include "../components.hpp"

inline auto new_main_menu_scene(rugame::SceneManager *scene_manager) -> rugame::Scene * {
  auto scene = new rugame::Scene{};

  scene->fn_init = [](rugame::Scene *) {
    rugame::SpriteMaterial::init();
  };

  scene->fn_deinit = [](rugame::Scene *) {
    rugame::SpriteMaterial::deinit();
  };

  scene->fn_start = [=](ruapp::Window *window, rugame::Scene *scene) {
    scene->ui_tree.root
      ->add((new rugui::Node{"title", "Example RPG"})->set_font_size(50)->set_flex_self_align(rugui::FlexAlign::Center))
      ->add(
        (new rugui::Node{"start game"})
          ->set_on_mouse_click_in([=](rugui::Node *, rugui::MouseButton button, int, int) {
            if (button == rugui::MouseButton::Left) {
              scene_manager->set_active_scene("game");
            }
            return false;
          })
          ->set_flex_self_align(rugui::FlexAlign::Center)
          ->set_width({rugui::SizeMode::FitContent, 0})
          ->set_height({rugui::SizeMode::FitContent, 0})
          ->add(
            (new rugui::Node{"text", "Start Game"})->set_font_size(30)->set_flex_self_align(rugui::FlexAlign::Center)))
      ->add(
        (new rugui::Node{"exit game"})
          ->set_on_mouse_click_in([=](rugui::Node *, rugui::MouseButton button, int, int) {
            if (button == rugui::MouseButton::Left) {
              window->should_close.store(true);
            }
            return false;
          })
          ->set_flex_self_align(rugui::FlexAlign::Center)
          ->set_width({rugui::SizeMode::FitContent, 0})
          ->set_height({rugui::SizeMode::FitContent, 0})
          ->add(
            (new rugui::Node{"text", "Exit Game"})->set_font_size(30)->set_flex_self_align(rugui::FlexAlign::Center)));
  };

  scene->fn_update = [=](ruapp::Window *window, rugame::Scene *scene, double delta) {
    if (window->is_key_just_down(VK_ESCAPE)) {
      scene_manager->set_active_scene("game");
    }
  };

  return scene;
}

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

  scene->fn_start = [](ruapp::Window *window, rugame::Scene *scene) {
    std::srand(std::time(nullptr));

    // init game objects
    for (auto i = 0; i < 10; ++i) {
      auto position = glm::vec3{rand() % 600 - 300, rand() % 600 - 300, 0.f};
      auto sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"monster.green_dragon"}};
      auto e = scene->archetype_storage.new_entity();
      scene->archetype_storage.add_component(e, TransformComponent{.position = position});
      scene->archetype_storage.add_component(e, SpriteComponent{.sprite = sprite});
      scene->archetype_storage.add_component(e, GreenDragonComponent{});
    }
    for (auto i = 0; i < 10; ++i) {
      auto position = glm::vec3{rand() % 600 - 300, rand() % 600 - 300, 0.f};
      auto sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"monster.red_dragon"}};
      auto e = scene->archetype_storage.new_entity();
      scene->archetype_storage.add_component(e, TransformComponent{.position = position});
      scene->archetype_storage.add_component(e, SpriteComponent{.sprite = sprite});
      scene->archetype_storage.add_component(e, RedDragonComponent{});
    }
    {
      auto sprite = new rugame::Sprite{{0.5f, 0.5f}, 100.f, 100.f, rugame::SpriteMaterial{"character.elf_warrior"}};
      auto e = scene->archetype_storage.new_entity();
      scene->archetype_storage.add_component(e, TransformComponent{});
      scene->archetype_storage.add_component(e, SpriteComponent{.sprite = sprite});
      scene->archetype_storage.add_component(e, ElfWarriorComponent{});
    }

    // init ui
    scene->ui_node_hashmap.insert(
      {"player",
       (new rugui::Node{"player", "health: "})->set_font_size(30)->set_flex_self_align(rugui::FlexAlign::Start)});

    scene->ui_tree.root
      ->add(
        (new rugui::Node{"title", "Example game"})->set_font_size(50)->set_flex_self_align(rugui::FlexAlign::Center))
      ->add(scene->ui_node_hashmap.at("player"));

    scene->update_systems.push_back( // render
      ruecs::System{{
                      typeid(TransformComponent).hash_code(),
                      typeid(SpriteComponent).hash_code(),
                    },
                    [=](ruecs::Entity entity, ruecs::Archetype &arch) {
                      auto transform = arch.get_component<TransformComponent>(entity);
                      auto sprite_component = arch.get_component<SpriteComponent>(entity);
                      auto sprite = sprite_component->sprite;
                      sprite->transform = glm::translate(glm::mat4{1}, transform->position);
                      scene->render_list.push_back(sprite);
                    }});

    scene->update_systems.push_back( // player
      ruecs::System{{
                      typeid(TransformComponent).hash_code(),
                      typeid(ElfWarriorComponent).hash_code(),
                    },
                    [=](ruecs::Entity entity, ruecs::Archetype &arch) {
                      auto transform = arch.get_component<TransformComponent>(entity);
                      auto elf_warrior = arch.get_component<ElfWarriorComponent>(entity);

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
                    }});
  };

  scene->fn_update = [=](ruapp::Window *window, rugame::Scene *scene, double delta) {
    if (window->is_key_just_down(VK_ESCAPE)) {
      scene_manager->set_active_scene("main_menu");
    }
  };

  return scene;
}
