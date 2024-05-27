#pragma once

#include <rubus-engine/game/scene.hpp>
#include <rubus-engine/game/resource.hpp>

inline auto new_main_menu_scene(rugame::SceneManager *scene_manager) -> rugame::Scene * {
  auto scene = new rugame::Scene{};

  scene->fn_on_init = [](rugame::Scene *) {
    rugame::SpriteMaterial::init();
  };

  scene->fn_on_deinit = [](rugame::Scene *) {
    rugame::SpriteMaterial::deinit();
  };

  scene->fn_on_start = [=](ruapp::Window *window, rugame::Scene *scene) {
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

  scene->fn_on_update = [=](ruapp::Window *window, rugame::Scene *, double) {
    if (window->is_key_just_down(VK_ESCAPE)) {
      scene_manager->set_active_scene("game");
    }
  };

  return scene;
}
