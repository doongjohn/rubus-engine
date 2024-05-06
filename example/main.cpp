#include <cstdlib>

#include "scenes/scene.hpp"

#define WIN32_LEAN_AND_MEAN
#include <rubus-engine/app/app.hpp>

auto WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) -> int {
  ruapp::attach_console();
  ruapp::load_wgl_functions();

  ruapp::Window::fiber_init();
  const auto window = ruapp::Window::create("Hello world", 800, 600);
  window->init_context();
  window->make_context_current();

  auto scene_manager = rugame::SceneManager{};
  auto test_scene = create_test_scene();
  scene_manager.set_active_scene(window, &test_scene);

  window->run([&](ruapp::Window *window, double delta) {
    scene_manager.scene->update(window, delta);
    scene_manager.scene->render(window, delta);
  });

  scene_manager.deinit();

  ruapp::Window::destroy(window);
  ruapp::Window::fiber_deinit();

  return EXIT_SUCCESS;
}
