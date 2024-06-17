#include <cstdlib>

#include "game/data.hpp"

#include "scenes/main_menu.hpp"
#include "scenes/character_selection_menu.hpp"
#include "scenes/game.hpp"

#include <rubus-engine/app/app.hpp>

auto WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int) -> int {
  ruapp::attach_console();

  ruapp::Window::init();
  ruapp::Window::fiber_init();

  const auto window = ruapp::Window::create("Hello world", 900, 600);
  window->init_context();
  window->make_context_current();

  auto game_data = GameData{};
  game_data.init();

  auto scene_manager = rugame::SceneManager{};
  scene_manager.register_scene("menu:main", new_main_menu_scene());
  scene_manager.register_scene("menu:character_selection", new_character_selection_menu_scene(&game_data));
  scene_manager.register_scene("game:game", new_game_scene(&game_data));
  scene_manager.set_active_scene("menu:main");

  window->run([&](ruapp::Window *window, double delta) {
    scene_manager.update(window, delta);
  });

  scene_manager.deinit(window);

  ruapp::Window::destroy(window);
  ruapp::Window::fiber_deinit();

  return EXIT_SUCCESS;
}
