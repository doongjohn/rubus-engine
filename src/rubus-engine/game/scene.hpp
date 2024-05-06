#pragma once

#include <functional>
#include <unordered_map>

#include <rubus-gui/screen.hpp>
#include <rubus-gui/renderer.hpp>
#include <rubus-gui/tree.hpp>
#include <rubus-engine/app/app.hpp>
#include "game.hpp"
#include "game_object.hpp"

namespace rugame {

struct Scene {
  Screen screen;
  Camera2d camera;
  std::vector<GameObject *> game_objects;
  std::vector<Sprite *> render_queue;

  rugui::Screen ui_screen;
  rugui::SkiaRenderer ui_renderer;
  rugui::Tree ui_tree;
  std::unordered_map<std::string, rugui::Node> ui_node_hashmap;

  std::function<void(Scene *scene)> fn_init;
  std::function<void(Scene *scene)> fn_deinit;
  std::function<void(ruapp::Window *window, Scene *scene)> fn_start;
  std::function<void(ruapp::Window *window, Scene *scene, double delta)> fn_update;

  auto init(ruapp::Window *window) -> void;
  auto deinit() -> void;

  auto update(ruapp::Window *window, double delta) -> void;
  auto render(ruapp::Window *window, double delta) -> void;
};

struct SceneManager {
  Scene *scene = nullptr;

  auto set_active_scene(ruapp::Window *window, Scene *const in_scene) -> void;
  auto deinit() -> void;
};

} // namespace game
