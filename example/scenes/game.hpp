#pragma once

#include <glm/ext/matrix_transform.hpp>

#include <rubus-engine/game/scene.hpp>
#include <rubus-engine/game/resource.hpp>

#include "../game/data.hpp"
#include "../components.hpp"

inline auto new_game_scene(GameData *game_data) -> rugame::Scene * {
  auto scene = new rugame::Scene{};

  scene->fn_on_init = [](rugame::Scene *) {
    // texture: backgrounds
    rugame::ResourceManager::load_texture2d_pixel("bg.plains-sheet1", "assets/bg/plains-sheet1.png");
    rugame::ResourceManager::load_texture2d_pixel("bg.plains-sheet2", "assets/bg/plains-sheet2.png");
    rugame::ResourceManager::load_texture2d_pixel("bg.plains-sheet3", "assets/bg/plains-sheet3.png");
    rugame::ResourceManager::load_texture2d_pixel("bg.plains-sheet4", "assets/bg/plains-sheet4.png");

    // texture: characters
    rugame::ResourceManager::load_texture2d_pixel("character.human_warrior", "assets/character/human_warrior.png");
    rugame::ResourceManager::load_texture2d_pixel("character.human_priest", "assets/character/human_priest.png");
    rugame::ResourceManager::load_texture2d_pixel("character.elf_archer", "assets/character/elf_archer.png");
    rugame::ResourceManager::load_texture2d_pixel("character.elf_mage", "assets/character/elf_mage.png");
    rugame::ResourceManager::load_texture2d_pixel("character.darkelf_assassin",
                                                  "assets/character/darkelf_assassin.png");

    // texture: monsters
    rugame::ResourceManager::load_texture2d_pixel("monster.green_dragon", "assets/monster/green_dragon.png");
    rugame::ResourceManager::load_texture2d_pixel("monster.red_dragon", "assets/monster/red_dragon.png");

    // material: sprite
    rugame::SpriteMaterial::init();
  };

  scene->fn_on_deinit = [=](rugame::Scene *) {
    rugame::ResourceManager::unload_texture2d_all();
    rugame::SpriteMaterial::deinit();
    game_data->reset();
  };

  scene->fn_on_start = [=](ruapp::Window *, rugame::SceneManager *, rugame::Scene *scene) {
    // spawn backgrounds
    {
      const auto w = 320.f * 3.5f;
      const auto h = 180.f * 3.5f;
      for (auto i = 1; i <= 4; ++i) {
        auto entity = scene->arch_storage.create_entity();
        auto position = glm::vec3{0, 0, 0};
        auto texture_id = std::format("bg.plains-sheet{}", i);
        auto sprite = new rugame::Sprite{{0.5f, 0.5f}, w, h, rugame::SpriteMaterial{texture_id}};
        sprite->zorder = -i - 1;
        entity.add_component<TransformComponent>(position);
        entity.add_component<SpriteComponent>(sprite);
      }
    }

    // spawn characters
    {
      auto spawn_pos = std::array{
        glm::vec3{-150, -130, 0},
        glm::vec3{-210, -130, 0},
        glm::vec3{-270, -130, 0},
        glm::vec3{-330, -130, 0},
      };

      const auto w = 50.f;
      const auto h = 50.f;
      for (auto i = 0; i < 4; ++i) {
        auto character = game_data->picked_characters[i];
        auto entity = scene->arch_storage.create_entity();
        auto position = spawn_pos[i];
        auto sprite = new rugame::Sprite{{0.5f, 0.5f}, w, h, rugame::SpriteMaterial{character.texture_id}};
        entity.add_component<TransformComponent>(position);
        entity.add_component<SpriteComponent>(sprite);
        entity.add_component<CharacterComponent>(character.name, character.health, character.ap);
      }
    }

    // spawn monster
    {
      auto entity = scene->arch_storage.create_entity();
      auto position = glm::vec3{150, -130 + 25, 0};
      auto sprite = new rugame::Sprite{{0.5f, 0.5f}, -90, 90, rugame::SpriteMaterial{"monster.red_dragon"}};
      entity.add_component<TransformComponent>(position);
      entity.add_component<SpriteComponent>(sprite);
    }

    // initialize ui
    auto node_character_data = (new rugui::Node{"character_data"})
                                 ->set_display_mode(rugui::DisplayMode::Hidden)
                                 ->set_margin(10)
                                 ->set_width({rugui::SizeMode::FitContent, 0})
                                 ->set_height({rugui::SizeMode::FitContent, 0});
    auto node_character_name = (new rugui::Node{"character_name", "Character name"})->set_font_size(25);
    auto node_character_hp = (new rugui::Node{"character_hp", "HP: 1"})->set_font_size(25);
    auto node_character_ap = (new rugui::Node{"character_ap", "AP: 1"})->set_font_size(25);

    scene->ui_nodes.insert({"character_data", node_character_data});
    scene->ui_nodes.insert({"character_name", node_character_name});
    scene->ui_nodes.insert({"character_hp", node_character_hp});
    scene->ui_nodes.insert({"character_ap", node_character_ap});

    scene->ui_tree.root->add(node_character_data //
                               ->add(node_character_name)
                               ->add(node_character_hp)
                               ->add(node_character_ap));
  };

  scene->fn_on_update = [](ruapp::Window *window, rugame::SceneManager *scene_manager, rugame::Scene *scene, double) {
    if (window->is_key_just_down(VK_ESCAPE)) {
      scene_manager->set_active_scene("menu:main");
    }

    static auto query_character = ruecs::Query{&scene->arch_storage}.with<TransformComponent, CharacterComponent>();
    static auto query_render = ruecs::Query{&scene->arch_storage}.with<TransformComponent, SpriteComponent>();

    for_each_entities(&scene->arch_storage, &scene->command, query_character) {
      auto transform = entity.get_component<TransformComponent>();
      auto character = entity.get_component<CharacterComponent>();
      if (window->is_mouse_just_down(rugui::MouseButton::Left)) {
        auto mouse_world_pos = scene->camera.screen_to_world_space({window->mouse_x, window->mouse_y});
        if (glm::distance(transform->position, {mouse_world_pos, 0}) <= 24.f) {
          scene->ui_nodes.at("character_data")->set_display_mode(rugui::DisplayMode::Shown);
          scene->ui_nodes.at("character_name")->text = character->name;
          scene->ui_nodes.at("character_hp")->text = std::format("HP: {}", character->health);
          scene->ui_nodes.at("character_ap")->text = std::format("AP: {}", character->ap);
        }
      }
    }

    scene->command.run();

    // render
    for_each_entities(&scene->arch_storage, &scene->command, query_render) {
      auto transform = entity.get_component<TransformComponent>();
      auto sprite_component = entity.get_component<SpriteComponent>();
      auto sprite = sprite_component->sprite;
      sprite->transform = glm::translate(glm::mat4{1}, transform->position);
      scene->sprites.push_back(sprite);
    }
  };

  return scene;
}
