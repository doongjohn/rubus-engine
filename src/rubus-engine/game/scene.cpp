#include "scene.hpp"

namespace rugame {

auto Scene::init(ruapp::Window *window) -> void {
  screen = Screen{(float)window->width, (float)window->height};
  camera = Camera2d{&screen, {0.f, 0.f, 3.f}};

  ui_screen.update_size(window->width, window->height);
  ui_renderer.init(&ui_screen);
  ui_tree.init(&ui_screen);

  window->on_resize = ([this](ruapp::Window *, int width, int height) {
    glViewport(0, 0, width, height);
    screen.width = (float)width;
    screen.height = (float)height;
    ui_screen.update_size(width, height);
    ui_renderer.regenerate_surface(&ui_screen);
    ui_tree.update_size(&ui_screen);
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

auto Scene::deinit() -> void {
  if (fn_deinit) {
    fn_deinit(this);
  }
  for (auto game_object : game_objects) {
    delete game_object;
  }
}

auto Scene::update(ruapp::Window *window, double delta) -> void {
  // update
  camera.update();
  if (fn_update) {
    fn_update(window, this, delta);
  }
  for (auto game_object : game_objects) {
    game_object->update(window, this, delta);
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
  for (auto sprite : render_queue) {
    sprite->draw(&camera);
  }
  render_queue.clear();

  // render rugui
  ui_renderer.context->resetContext();
  ui_tree.root->layout(&ui_renderer);
  ui_tree.root->draw_all(&ui_renderer);
  ui_renderer.flush();

  // swap buffers
  window->swap_buffers();
}

auto SceneManager::set_active_scene(ruapp::Window *window, Scene *const in_scene) -> void {
  if (in_scene == nullptr) {
    return;
  }

  if (scene != nullptr) {
    scene->deinit();
  }

  scene = in_scene;
  scene->init(window);
  if (scene->fn_init) {
    scene->fn_init(scene);
  }
  if (scene->fn_start) {
    scene->fn_start(window, scene);
  }
}

auto SceneManager::deinit() -> void {
  if (scene != nullptr) {
    scene->deinit();
  }
}

} // namespace rugame
