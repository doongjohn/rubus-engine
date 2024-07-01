#pragma once

#include <atomic>
#include <functional>
#include <optional>
#include <tuple>
#include <string_view>

#include <glad/glad.h>

#include <rubus-gui/base.hpp>

#include <windows.h>
#include "wglext.h"

namespace ruapp {

static constexpr auto WM_USER_RESUME = WM_USER + 0x0001;

static auto wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC) nullptr;
static auto wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) nullptr;
static auto wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) nullptr;

struct Window {
  inline static int window_count = 0;
  inline static LPVOID main_fiber = nullptr;
  inline static LPVOID message_loop_fiber = nullptr;

  HWND hWnd = nullptr;
  HMODULE hInstance = nullptr;
  LPCWSTR win_class_name = nullptr;
  HDC device_ctx = nullptr;
  HGLRC rendering_ctx = nullptr;

  int width = 0;
  int height = 0;

  std::optional<std::tuple<int, int>> size_min;
  std::optional<std::tuple<int, int>> size_max;

  bool is_mouse_tracking = false;
  std::atomic_bool should_close = false;
  std::atomic_bool send_resume_message = false;

  int mouse_x = 0;
  int mouse_y = 0;
  std::array<bool, 3> mouse_states{};
  std::array<uint8_t, 3> mouse_just_states{};

  // https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
  std::array<bool, 0xFE + 1> key_states{};
  std::array<uint8_t, 0xFE + 1> key_just_states{};

  std::function<void(Window *, int, int)> on_resize;
  std::function<void(Window *, int, int)> on_mouse_enter;
  std::function<void(Window *, int, int)> on_mouse_leave;
  std::function<void(Window *, int, int)> on_mouse_move;
  std::function<void(Window *, rugui::MouseButton, int, int)> on_mouse_down;
  std::function<void(Window *, rugui::MouseButton, int, int)> on_mouse_up;
  std::function<void(Window *, int)> on_mouse_scroll;
  std::function<void(Window *, uint32_t)> on_key_down;
  std::function<void(Window *, uint32_t)> on_key_up;

  static auto init() -> void;
  static auto create(std::string_view title, int width, int height) -> Window *;
  static auto destroy(Window *window) -> void;

  auto init_context() -> void;
  auto make_context_current() -> void;
  auto swap_buffers() -> void;

  auto run(const std::function<void(Window *, double)> &fn_update) -> void;

  auto is_mouse_down(rugui::MouseButton mouse_button) -> bool;
  auto is_mouse_up(rugui::MouseButton mouse_button) -> bool;
  auto is_mouse_just_down(rugui::MouseButton mouse_button) -> bool;
  auto is_mouse_just_up(rugui::MouseButton mouse_button) -> bool;

  auto is_key_down(uint32_t keycode) -> bool;
  auto is_key_up(uint32_t keycode) -> bool;
  auto is_key_just_down(uint32_t keycode) -> bool;
  auto is_key_just_up(uint32_t keycode) -> bool;
};

auto attach_console() -> void;
auto reset_context() -> void;

auto get_current_tick() -> double;
auto get_tick_per_sec() -> double;

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

} // namespace ruapp
