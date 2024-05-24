#include <cstdlib>

#include "scenes/main_menu.hpp"
#include "scenes/game.hpp"

#include <rubus-engine/app/app.hpp>

auto WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) -> int {
  ruapp::attach_console();
  ruapp::load_wgl_functions();

  ruapp::Window::fiber_init();
  const auto window = ruapp::Window::create("Hello world", 800, 600);
  window->init_context();
  window->make_context_current();

  auto scene_manager = rugame::SceneManager{};
  scene_manager.register_scene("main_menu", new_main_menu_scene(&scene_manager));
  scene_manager.register_scene("game", new_game_scene(&scene_manager));
  scene_manager.set_active_scene("main_menu");

  window->run([&](ruapp::Window *window, double delta) {
    scene_manager.update(window, delta);
  });

  scene_manager.deinit(window);

  ruapp::Window::destroy(window);
  ruapp::Window::fiber_deinit();

  return EXIT_SUCCESS;
}
