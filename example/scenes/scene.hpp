#pragma once

#include <rubus-engine/game/scene.hpp>
#include <rubus-engine/game/resource.hpp>
#include "../game_object.hpp"

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

  scene->fn_start = [](ruapp::Window *, rugame::Scene *scene) {
    std::srand(std::time(nullptr));

    // init game objects
    for (auto i = 0; i < 10; ++i) {
      scene->instantiate<GreenDragon>();
    }
    for (auto i = 0; i < 10; ++i) {
      scene->instantiate<RedDragon>();
    }
    scene->instantiate<ElfWarrior>();

    // init ui
    scene->ui_node_hashmap.insert(
      {"player",
       (new rugui::Node{"player", "health: "})->set_font_size(30)->set_flex_self_align(rugui::FlexAlign::Start)});

    scene->ui_tree.root
      ->add(
        (new rugui::Node{"title", "Example game"})->set_font_size(50)->set_flex_self_align(rugui::FlexAlign::Center))
      ->add(scene->ui_node_hashmap.at("player"));
  };

  scene->fn_update = [=](ruapp::Window *window, rugame::Scene *scene, double delta) {
    if (window->is_key_just_down(VK_ESCAPE)) {
      scene_manager->set_active_scene("main_menu");
    }
  };

  return scene;
}
