#include "scene.hpp"

namespace rugame {

auto Scene::init(ruapp::Window *window) -> void {
  screen = Screen{(float)window->width, (float)window->height};
  camera = Camera2d{&screen, {0.f, 0.f, 3.f}};
  archetype_storage = {};

  ui_screen.set_size(window->width, window->height);
  ui_renderer.init(&ui_screen);
  ui_tree.init(&ui_screen);

  window->on_resize = ([this](ruapp::Window *, int width, int height) {
    glViewport(0, 0, width, height);
    screen.width = (float)width;
    screen.height = (float)height;
    ui_screen.set_size(width, height);
    ui_renderer.regenerate_surface(&ui_screen);
    ui_tree.set_size(&ui_screen);
  });
  window->on_mouse_enter = ([this](ruapp::Window *, int x, int y) {
    ui_tree.run_mouse_event(x, y);
  });
  window->on_mouse_leave = ([this](ruapp::Window *, int x, int y) {
    ui_tree.run_mouse_leave_window_event(x, y);
  });
  window->on_mouse_move = ([this](ruapp::Window *, int x, int y) {
    ui_tree.run_mouse_event(x, y);
  });
  window->on_mouse_down = ([this](ruapp::Window *, rugui::MouseButton button, int x, int y) {
    ui_tree.run_mouse_down_event(button, x, y);
  });
  window->on_mouse_up = ([this](ruapp::Window *, rugui::MouseButton button, int x, int y) {
    ui_tree.run_mouse_up_event(button, x, y);
  });
  window->on_mouse_scroll = ([this](ruapp::Window *, int delta) {
    ui_tree.run_vscroll_event((float)delta * 0.2f); // NOLINT
  });
}

auto Scene::deinit(ruapp::Window *window) -> void {
  window->on_resize = nullptr;
  window->on_mouse_enter = nullptr;
  window->on_mouse_leave = nullptr;
  window->on_mouse_move = nullptr;
  window->on_mouse_down = nullptr;
  window->on_mouse_up = nullptr;
  window->on_mouse_scroll = nullptr;

  if (fn_deinit) {
    fn_deinit(this);
  }

  archetype_storage.archetypes.clear();
  oneshot_systems.clear();
  update_systems.clear();
  render_list.clear();

  ui_node_hashmap.clear();
  ui_tree.reset();
}

auto Scene::destroy_entity(ruecs::Entity entity) -> void {
  destroy_entities.insert(entity);
}

auto Scene::update_pre() -> void {
  for (auto entity : destroy_entities) {
    archetype_storage.delete_entity(entity);
  }
  destroy_entities.clear();
}

auto Scene::update(ruapp::Window *window, double delta) -> void {
  delta_time = delta;
  camera.update();

  // scene upate
  if (fn_update) {
    fn_update(window, this, delta);
  }

  // oneshot systems
  for (const auto &system : oneshot_systems) {
    archetype_storage.run_system(system);
  }
  oneshot_systems.clear();

  // update system
  for (const auto &system : update_systems) {
    archetype_storage.run_system(system);
  }
}

auto Scene::render(ruapp::Window *window, double) -> void {
  // clear gui
  ui_renderer.clear(SkColors::kTransparent);
  ui_renderer.flush();

  // opengl settings
  glDisable(GL_FRAMEBUFFER_SRGB);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // clear
  glClearColor(1.f, 1.f, 1.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // render game
  for (auto sprite : render_list) {
    sprite->draw(&camera);
  }
  render_list.clear();

  // render gui
  ui_renderer.context->resetContext();
  ui_tree.root->layout(&ui_renderer);
  ui_tree.root->draw_all(&ui_renderer);
  ui_renderer.flush();

  // swap buffers
  window->swap_buffers();
}

auto SceneManager::register_scene(const std::string &name, Scene *scene) -> void {
  scenes.insert({name, scene});
}

auto SceneManager::unregister_scene(const std::string &name) -> void {
  scenes.erase(name);
}

auto SceneManager::set_active_scene(const std::string &name) -> void {
  if (not scenes.contains(name) || cur_scene == scenes.at(name)) {
    return;
  }
  new_scene = scenes.at(name);
}

auto SceneManager::change_scene(ruapp::Window *window) -> void {
  if (new_scene == nullptr) {
    return;
  }

  // deinit current scene
  if (cur_scene != nullptr) {
    cur_scene->deinit(window);
  }

  cur_scene = new_scene;
  new_scene = nullptr;

  // init new scene
  cur_scene->init(window);
  if (cur_scene->fn_init) {
    cur_scene->fn_init(cur_scene);
  }
  if (cur_scene->fn_start) {
    cur_scene->fn_start(window, cur_scene);
  }

  // init ui
  {
    auto mouse_pos = POINT{};
    ::GetCursorPos(&mouse_pos);
    ::ScreenToClient(window->hWnd, &mouse_pos);
    cur_scene->ui_tree.root->layout(&cur_scene->ui_renderer);
    cur_scene->ui_tree.run_mouse_event(mouse_pos.x, mouse_pos.y);
  }
}

auto SceneManager::deinit(ruapp::Window *window) -> void {
  if (cur_scene != nullptr) {
    cur_scene->deinit(window);
  }
  for (const auto &[_, scene] : scenes) {
    delete scene;
  }
}

auto SceneManager::update(ruapp::Window *window, double delta) -> void {
  if (cur_scene != nullptr) {
    cur_scene->update_pre();
    cur_scene->update(window, delta);
    cur_scene->render(window, delta);
  }
  change_scene(window);
}

} // namespace rugame
