#pragma once

#include <rubus-engine/game/scene.hpp>
#include <rubus-engine/game/resource.hpp>
#include "../game_object.hpp"

inline auto create_test_scene() -> rugame::Scene {
  auto scene = rugame::Scene{};

  scene.fn_init = [](rugame::Scene *) {
    rugame::ResourceManager::load_texture2d_pixel("character.elf_warrior", "assets/character/elf_warrior.png");
    rugame::ResourceManager::load_texture2d_pixel("monster.green_dragon", "assets/monster/green_dragon.png");
    rugame::SpriteMaterial::init();
  };

  scene.fn_deinit = [](rugame::Scene *) {
    rugame::ResourceManager::unload_texture2d();
    rugame::SpriteMaterial::deinit();
  };

  scene.fn_start = [](ruapp::Window *, rugame::Scene *scene) {
    srand(time(nullptr));
    for (auto i = 0; i < 20; ++i) {
      scene->game_objects.push_back(new Dragon{});
    }
    scene->game_objects.push_back(new ElfWarrior{});

    scene->ui_tree.root->add(
      (new rugui::Node{"title", "Example game"})->set_font_size(50)->set_flex_self_align(rugui::FlexAlign::Center));
  };

  // scene.fn_update = [](app::Window *window, game::Scene *scene, double delta) {};

  return scene;
}
