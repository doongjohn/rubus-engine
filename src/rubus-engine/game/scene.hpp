#pragma once

#include <functional>
#include <unordered_map>

#include <rubus-gui/screen.hpp>
#include <rubus-gui/renderer.hpp>
#include <rubus-gui/tree.hpp>
#include <rubus-ecs/ecs.hpp>

#include <rubus-engine/app/app.hpp>
#include "game.hpp"

namespace rugame {

struct SceneManager;

struct Scene {
  Screen screen;
  Camera2d camera;
  std::vector<Sprite *> sprites;

  ruecs::ArchetypeStorage arch_storage;
  ruecs::Command command;

  rugui::Screen ui_screen;
  rugui::SkiaRenderer ui_renderer;
  rugui::Tree ui_tree;
  std::unordered_map<std::string, rugui::Node *> ui_nodes;

  double delta = 0;

  using Callback1 = std::function<void(Scene *scene)>;
  using Callback2 = std::function<void(ruapp::Window *window, SceneManager *scene_manager, Scene *scene)>;
  using Callback3 = std::function<void(ruapp::Window *window, SceneManager *scene_manager, Scene *scene, double delta)>;

  Callback1 fn_on_init;
  Callback1 fn_on_deinit;
  Callback2 fn_on_start;
  Callback1 fn_on_end;
  Callback3 fn_on_update;

  Scene();

  auto init(ruapp::Window *window) -> void;
  auto deinit(ruapp::Window *window) -> void;
  auto update(ruapp::Window *window, SceneManager *scene_manager, double delta) -> void;
  auto render(ruapp::Window *window, double delta) -> void;
};

struct SceneManager {
  Scene *cur_scene = nullptr;
  Scene *new_scene = nullptr;
  std::unordered_map<std::string, Scene *> scenes;

  auto deinit(ruapp::Window *window) -> void;
  auto update(ruapp::Window *window, double delta) -> void;

  auto register_scene(const std::string &name, Scene *scene) -> void;
  auto unregister_scene(const std::string &name) -> void;
  auto set_active_scene(const std::string &name) -> void;
  auto change_scene(ruapp::Window *window) -> void;
};

} // namespace rugame
