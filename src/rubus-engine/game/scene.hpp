#pragma once

#include <functional>
#include <unordered_set>
#include <unordered_map>

#include <rubus-gui/screen.hpp>
#include <rubus-gui/renderer.hpp>
#include <rubus-gui/tree.hpp>
#include <rubus-ecs/ecs.hpp>

#include <rubus-engine/app/app.hpp>
#include "game.hpp"

namespace rugame {

struct Scene {
  Screen screen;
  double delta_time = 0;

  Camera2d camera;
  ruecs::ArchetypeStorage arch_storage;
  std::unordered_set<ruecs::Entity> destroy_entities;
  std::vector<Sprite *> render_list;

  rugui::Screen ui_screen;
  rugui::SkiaRenderer ui_renderer;
  rugui::Tree ui_tree;
  std::unordered_map<std::string, rugui::Node *> ui_node_hashmap;

  std::function<void(Scene *scene)> fn_init;
  std::function<void(Scene *scene)> fn_deinit;
  std::function<void(ruapp::Window *window, Scene *scene)> fn_start;
  std::function<void(ruapp::Window *window, Scene *scene, double delta)> fn_update;

  auto init(ruapp::Window *window) -> void;
  auto deinit(ruapp::Window *window) -> void;

  auto destroy_entity(ruecs::Entity entity) -> void;

  auto update_pre() -> void;
  auto update(ruapp::Window *window, double delta) -> void;
  auto render(ruapp::Window *window, double delta) -> void;
};

struct SceneManager {
  Scene *cur_scene = nullptr;
  Scene *new_scene = nullptr;
  std::unordered_map<std::string, Scene *> scenes;

  auto register_scene(const std::string &name, Scene *scene) -> void;
  auto unregister_scene(const std::string &name) -> void;

  auto set_active_scene(const std::string &name) -> void;
  auto change_scene(ruapp::Window *window) -> void;
  auto deinit(ruapp::Window *window) -> void;

  auto update(ruapp::Window *window, double delta) -> void;
};

} // namespace rugame
