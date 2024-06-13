#include "scene.hpp"

#include <iostream>

namespace rugame {

Scene::Scene() : command{&arch_storage} {}

auto Scene::init(ruapp::Window *window) -> void {
  screen = Screen{(float)window->width, (float)window->height};
  camera = Camera2d{&screen, {0.f, 0.f, 10.f}};

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
  if (fn_on_end) {
    fn_on_end(this);
  }

  sprites.clear();

  arch_storage.delete_all_archetypes();
  command.discard();

  ui_nodes.clear();
  ui_tree.reset();

  window->on_resize = nullptr;
  window->on_mouse_enter = nullptr;
  window->on_mouse_leave = nullptr;
  window->on_mouse_move = nullptr;
  window->on_mouse_down = nullptr;
  window->on_mouse_up = nullptr;
  window->on_mouse_scroll = nullptr;

  if (fn_on_deinit) {
    fn_on_deinit(this);
  }
}

auto Scene::update(ruapp::Window *window, SceneManager *scene_manager, double delta) -> void {
  this->delta = delta;
  camera.update();

  // scene upate
  if (fn_on_update) {
    fn_on_update(window, scene_manager, this, delta);
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

  // render sprites
  std::ranges::sort(sprites, [](Sprite *a, Sprite *b) {
    return a->zorder < b->zorder;
  });
  for (auto sprite : sprites) {
    sprite->draw(&camera);
  }
  sprites.clear();

  // render gui
  ui_renderer.context->resetContext();
  ui_tree.root->layout(&ui_renderer);
  ui_tree.root->draw_all(&ui_renderer);
  ui_renderer.flush();

  // swap buffers
  window->swap_buffers();
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
    cur_scene->update(window, this, delta);
    cur_scene->render(window, delta);
  }
  change_scene(window);
}

auto SceneManager::register_scene(const std::string &name, Scene *scene) -> void {
  scenes.insert({name, scene});
}

auto SceneManager::unregister_scene(const std::string &name) -> void {
  scenes.erase(name);
}

auto SceneManager::set_active_scene(const std::string &name) -> void {
  if (not scenes.contains(name)) {
    std::cout << std::format("set_active_scene failed: unknown registered scene name \"{}\"", name);
    return;
  }
  if (cur_scene != scenes.at(name)) {
    new_scene = scenes.at(name);
  }
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
  if (cur_scene->fn_on_init) {
    cur_scene->fn_on_init(cur_scene);
  }
  if (cur_scene->fn_on_start) {
    cur_scene->fn_on_start(window, this, cur_scene);
  }

  // init ui
  auto mouse_pos = POINT{};
  ::GetCursorPos(&mouse_pos);
  ::ScreenToClient(window->hWnd, &mouse_pos);
  cur_scene->ui_tree.root->layout(&cur_scene->ui_renderer);
  cur_scene->ui_tree.run_mouse_event(mouse_pos.x, mouse_pos.y);
}

} // namespace rugame
