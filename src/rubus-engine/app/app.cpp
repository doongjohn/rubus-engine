#include "app.hpp"

#include <thread>
#include <array>
#include <format>
#include <iostream>

#include <commctrl.h>
#include "wglext.h"

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif

#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

namespace ruapp {

auto Window::init() -> void {
  // https://gist.github.com/mmozeiko/ed2ad27f75edf9c26053ce332a1f6647
  auto dummy = ::CreateWindowExW(0, L"STATIC", L"DummyWindow", WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT,
                                 CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, nullptr, nullptr);
  auto dc = ::GetDC(dummy);

  auto pfd = PIXELFORMATDESCRIPTOR{};
  pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;

  auto pf = ::ChoosePixelFormat(dc, &pfd);
  if (pf == 0) {
    std::cerr << "Error: load_wgl_functions ChoosePixelFormat failed\n";
  }
  if (not ::DescribePixelFormat(dc, pf, sizeof(pfd), &pfd)) {
    std::cerr << "Error: load_wgl_functions DescribePixelFormat failed\n";
  }
  if (not ::SetPixelFormat(dc, pf, &pfd)) {
    std::cerr << "Error: load_wgl_functions SetPixelFormat failed\n";
  }

  auto rc = ::wglCreateContext(dc);
  if (rc == nullptr) {
    std::cerr << "Error: load_wgl_functions wglCreateContext failed\n";
  }
  if (not ::wglMakeCurrent(dc, rc)) {
    std::cerr << "Error: load_wgl_functions wglMakeCurrent failed\n";
  }

  wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)::wglGetProcAddress("wglChoosePixelFormatARB");
  if (wglChoosePixelFormatARB == nullptr) {
    std::cerr << "Error: load_wgl_functions \"wglChoosePixelFormatARB\" loading failed\n";
  }

  wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)::wglGetProcAddress("wglCreateContextAttribsARB");
  if (wglCreateContextAttribsARB == nullptr) {
    std::cerr << "Error: load_wgl_functions \"wglCreateContextAttribsARB\" loading failed\n";
  }

  wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)::wglGetProcAddress("wglSwapIntervalEXT");
  if (wglSwapIntervalEXT == nullptr) {
    std::cerr << "Error: load_wgl_functions \"wglSwapIntervalEXT\" loading failed\n";
  }

  ::wglMakeCurrent(nullptr, nullptr);
  ::wglDeleteContext(rc);
  ::ReleaseDC(dummy, dc);
  ::DestroyWindow(dummy);
}

auto Window::fiber_init() -> void {
  main_fiber = ::ConvertThreadToFiber(nullptr);
  message_loop_fiber = ::CreateFiber(
    0,
    [](void *) -> void {
      auto msg = MSG{};
      while (Window::window_count > 0) {
        while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
          ::TranslateMessage(&msg);
          ::DispatchMessageW(&msg);
        }
        ::SwitchToFiber(Window::main_fiber);
      }
      ::SwitchToFiber(Window::main_fiber);
    },
    nullptr);
}

auto Window::fiber_deinit() -> void {
  ::DeleteFiber(message_loop_fiber);
  ::ConvertFiberToThread();
}

auto Window::create(std::string_view title, int width, int height) -> Window * {
  auto window = new Window{};
  window->hInstance = ::GetModuleHandleW(nullptr);
  window->win_class_name = L"AppWindowClass";

  window->width = width;
  window->height = height;

  // convert string to wstring
  const auto utf8_len = static_cast<int>(title.length());
  const auto utf16_len = ::MultiByteToWideChar(CP_UTF8, 0, title.data(), utf8_len, nullptr, 0);

  auto title_utf16 = std::wstring(utf16_len, '\0');
  ::MultiByteToWideChar(CP_UTF8, 0, title.data(), utf8_len, title_utf16.data(), utf16_len);

  WNDCLASSEXW wc = {};
  wc.cbSize = sizeof(wc);
  wc.lpfnWndProc = WndProc;
  wc.hInstance = window->hInstance;
  wc.hIcon = ::LoadIconA(nullptr, IDI_APPLICATION);
  wc.hCursor = ::LoadCursorA(nullptr, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);
  wc.lpszClassName = window->win_class_name;
  ::RegisterClassExW(&wc);

  auto window_rect = RECT{0, 0, width, height};
  ::AdjustWindowRectEx(&window_rect, WS_OVERLAPPEDWINDOW, FALSE, 0);
  const auto window_width = window_rect.right - window_rect.left;
  const auto window_height = window_rect.bottom - window_rect.top;

  // clang-format off
  window->hWnd = ::CreateWindowExW(
    0,                   // Optional window styles
    wc.lpszClassName,    // Window class
    title_utf16.data(),  // Window text
    WS_OVERLAPPEDWINDOW, // Window style

    // xpos, ypos, xsize, ysize
    CW_USEDEFAULT, CW_USEDEFAULT, window_width, window_height,

    nullptr,           // Parent window
    nullptr,           // Menu
    window->hInstance, // Instance handle
    nullptr            // Additional application data
  );
  // clang-format on

  if (window->hWnd == nullptr) {
    std::cerr << "Error: failed to create window\n";
    return window;
  }

  ::SetPropW(window->hWnd, L"AppObj", window);
  ::ShowWindow(window->hWnd, SW_SHOW);

  auto dc = ::GetDC(window->hWnd);
  window->device_ctx = dc;

  Window::window_count += 1;
  std::cout << "Window created\n";

  return window;
}

auto Window::destroy(Window *window) -> void {
  ::UnregisterClassW(window->win_class_name, window->hInstance);
  ::DestroyWindow(window->hWnd);
  delete window;

  Window::window_count -= 1;
  std::cout << "Window destroyed";
}

auto Window::init_context() -> void {
  auto dc = device_ctx;

  // set pixel format
  {
    // clang-format off
    int attribs[] = {
      WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
      WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
      WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
      WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
      WGL_COLOR_BITS_ARB, 24,
      WGL_DEPTH_BITS_ARB, 24,
      WGL_STENCIL_BITS_ARB, 8,

      // sRGB framebuffer, from WGL_ARB_framebuffer_sRGB extension
      // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_framebuffer_sRGB.txt
      WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB, GL_TRUE,

      // multisampeld framebuffer, from WGL_ARB_multisample extension
      // https://www.khronos.org/registry/OpenGL/extensions/ARB/ARB_multisample.txt
      // WGL_SAMPLE_BUFFERS_ARB, 1,
      // WGL_SAMPLES_ARB,        4, // 4x MSAA

      0,
    };
    // clang-format on
    auto format = 0;
    auto formats = uint32_t{};
    if (not wglChoosePixelFormatARB(dc, attribs, nullptr, 1, &format, &formats) or formats == 0) {
      std::cerr << "wglChoosePixelFormatARB failed\n";
      return;
    }
    auto pfd = PIXELFORMATDESCRIPTOR{};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    if (not ::DescribePixelFormat(dc, format, sizeof(pfd), &pfd)) {
      std::cerr << "DescribePixelFormat failed\n";
      return;
    }
    if (not ::SetPixelFormat(dc, format, &pfd)) {
      std::cerr << "SetPixelFormat failed\n";
      return;
    }
  }

  // create opengl context
  {
    // clang-format off
    int attribs[] = {
      WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
      WGL_CONTEXT_MINOR_VERSION_ARB, 6,
      WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
      WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
      0,
    };
    // clang-format on
    auto rc = wglCreateContextAttribsARB(dc, nullptr, attribs);
    if (rc == nullptr) {
      std::cerr << "wglCreateContextAttribsARB failed\n";
      return;
    }
    rendering_ctx = rc;
    std::cout << "OpenGL context created\n";
  }

  // setup debug callback
  make_context_current();
  {
    auto flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    auto is_debug = flags & GL_CONTEXT_FLAG_DEBUG_BIT;
    if (is_debug) {
      glEnable(GL_DEBUG_OUTPUT);
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
      glDebugMessageCallback(
        [](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *msg,
           const void *data) -> void {
          (void)length;
          (void)data;

          if (id == 131186) {
            // Buffer performance warning: Buffer object 1 (bound to GL_ELEMENT_ARRAY_BUFFER_ARB, usage hint is GL_STATIC_DRAW) is being copied/moved from VIDEO memory to HOST memory.
            return;
          }

          const char *str_source;
          switch (source) {
          case GL_DEBUG_SOURCE_API:
            str_source = "API";
            break;
          case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            str_source = "WINDOW_SYSTEM";
            break;
          case GL_DEBUG_SOURCE_SHADER_COMPILER:
            str_source = "SHADER_COMPILER";
            break;
          case GL_DEBUG_SOURCE_THIRD_PARTY:
            str_source = "THIRD_PARTY";
            break;
          case GL_DEBUG_SOURCE_APPLICATION:
            str_source = "APPLICATION";
            break;
          case GL_DEBUG_SOURCE_OTHER:
            str_source = "OTHER";
            break;
          default:
            str_source = "UNKNOWN";
            break;
          }

          const char *str_type;
          switch (type) {
          case GL_DEBUG_TYPE_ERROR:
            str_type = "ERROR";
            break;
          case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            str_type = "DEPRECATED_BEHAVIOR";
            break;
          case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            str_type = "UDEFINED_BEHAVIOR";
            break;
          case GL_DEBUG_TYPE_PORTABILITY:
            str_type = "PORTABILITY";
            break;
          case GL_DEBUG_TYPE_PERFORMANCE:
            str_type = "PERFORMANCE";
            break;
          case GL_DEBUG_TYPE_OTHER:
            str_type = "OTHER";
            break;
          case GL_DEBUG_TYPE_MARKER:
            str_type = "MARKER";
            break;
          default:
            str_type = "UNKNOWN";
            break;
          }

          const char *str_severity;
          switch (severity) {
          case GL_DEBUG_SEVERITY_HIGH:
            str_severity = "HIGH";
            break;
          case GL_DEBUG_SEVERITY_MEDIUM:
            str_severity = "MEDIUM";
            break;
          case GL_DEBUG_SEVERITY_LOW:
            str_severity = "LOW";
            break;
          case GL_DEBUG_SEVERITY_NOTIFICATION:
            str_severity = "NOTIFICATION";
            break;
          default:
            str_severity = "UNKNOWN";
            break;
          }

          std::cerr << std::format("OpenGL debug message [{}]: ({}, {}) from {}\n  {}\n", str_severity, str_type, id,
                                   str_source, msg);
        },
        nullptr);
      std::cout << "OpenGL debug callback enabled\n";
    } else {
      std::cout << "OpenGL debug callback not enabled\n";
    }
  }
  reset_context();
}

auto Window::make_context_current() -> void {
  ::wglMakeCurrent(device_ctx, rendering_ctx);
  // wglSwapIntervalEXT(0);
  gladLoadGL();
}

auto Window::swap_buffers() -> void {
  ::SwapBuffers(device_ctx);
}

auto Window::run(const std::function<void(Window *, double)> &fn_update) -> void {
  const auto resume_message_thread = std::jthread{[this]() {
    while (not should_close.load()) {
      while (send_resume_message.load()) {
        ::SendMessageW(hWnd, WM_USER_RESUME, 0, 0);
      }
    }
  }};

  const auto tick_per_sec = get_tick_per_sec();
  auto last_frame = get_current_tick();
  auto delta_time = double{};
  auto fps_timer = double{};
  auto fps = uint64_t{};

  // main loop
  while (not should_close.load()) {
    // calculate delta time
    delta_time = (get_current_tick() - last_frame) / tick_per_sec;
    last_frame = get_current_tick();

    // calculate fps
    {
      fps_timer += delta_time;
      fps += 1;
      if (fps_timer >= 1) {
        ::SetWindowTextW(hWnd, std::format(L"FPS {}", fps).data());
        fps = 0;
        fps_timer = 0;
      }
    }

    // handle message
    ::SwitchToFiber(message_loop_fiber);

    // update
    fn_update(this, delta_time);

    // reset just states
    std::memset(mouse_just_states.data(), 0, mouse_just_states.size());
    std::memset(key_just_states.data(), 0, key_just_states.size());
  }
}

auto Window::is_mouse_down(rugui::MouseButton mouse_button) -> bool {
  return mouse_states[(int)mouse_button];
}

auto Window::is_mouse_up(rugui::MouseButton mouse_button) -> bool {
  return not mouse_states[(int)mouse_button];
}

auto Window::is_mouse_just_down(rugui::MouseButton mouse_button) -> bool {
  return mouse_just_states[(int)mouse_button] == 1;
}

auto Window::is_mouse_just_up(rugui::MouseButton mouse_button) -> bool {
  return mouse_just_states[(int)mouse_button] == 2;
}

auto Window::is_key_down(uint32_t keycode) -> bool {
  return key_states[keycode];
}

auto Window::is_key_up(uint32_t keycode) -> bool {
  return not key_states[keycode];
}

auto Window::is_key_just_down(uint32_t keycode) -> bool {
  return key_just_states[keycode] == 1;
}

auto Window::is_key_just_up(uint32_t keycode) -> bool {
  return key_just_states[keycode] == 2;
}

auto attach_console() -> void {
  // https://stackoverflow.com/questions/15543571/allocconsole-not-displaying-cout
  if (::AttachConsole(ATTACH_PARENT_PROCESS)) {
    freopen_s((FILE **)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE **)stderr, "CONOUT$", "w", stderr);
  }
}

auto reset_context() -> void {
  ::wglMakeCurrent(nullptr, nullptr);
}

auto get_current_tick() -> double {
  auto value = uint64_t{};
  ::QueryPerformanceCounter((LARGE_INTEGER *)&value);
  return (double)value;
}

auto get_tick_per_sec() -> double {
  auto frequency = uint64_t{};
  ::QueryPerformanceFrequency((LARGE_INTEGER *)&frequency);
  return (double)frequency;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  const auto window = (Window *)::GetPropW(hWnd, L"AppObj");
  if (window == nullptr) {
    return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
  }

  switch (uMsg) {
  case WM_CLOSE: {
    window->should_close.store(true);
    return 0;
  } break;

  case WM_DESTROY: {
    window->should_close.store(true);
    ::PostQuitMessage(0);
    return 0;
  } break;

  case WM_GETMINMAXINFO: {
    const auto minmax = (MINMAXINFO *)lParam; // NOLINT
    if (window->size_min.has_value()) {
      auto [x, y] = window->size_min.value();
      minmax->ptMinTrackSize.x = x;
      minmax->ptMinTrackSize.y = y;
    }
    if (window->size_max.has_value()) {
      auto [x, y] = window->size_max.value();
      minmax->ptMaxTrackSize.x = x;
      minmax->ptMaxTrackSize.y = y;
    }
    return 0;
  } break;

  case WM_NCLBUTTONDOWN: {
    // this fixes 500ms blocking issue when holding the title bar
    switch (wParam) {
    case HTCAPTION:
      ::PostMessageW(hWnd, WM_MOUSEMOVE, 0, 0);
      break;
    }
  } break;

    // modal loop enter
  case WM_ENTERSIZEMOVE:
  case WM_ENTERMENULOOP: {
    window->send_resume_message.store(true);
    break;
  }

    // modal loop exit
  case WM_EXITSIZEMOVE:
  case WM_EXITMENULOOP: {
    window->send_resume_message.store(false);
    break;
  }

    // switch to main when running modal loop
  case WM_USER_RESUME: {
    ::SwitchToFiber(window->main_fiber);
    return 0;
  } break;

    // window resize
  case WM_SIZE: {
    const auto width = LOWORD(lParam);
    const auto height = HIWORD(lParam);
    window->width = width;
    window->height = height;
    if (window->on_resize) {
      window->on_resize(window, width, height);
    }
  } break;

  case WM_MOUSEHOVER: {
    window->is_mouse_tracking = false;
    TRACKMOUSEEVENT tme;
    tme.cbSize = sizeof(tme);
    tme.hwndTrack = hWnd;
    tme.dwFlags = TME_LEAVE;
    ::_TrackMouseEvent(&tme);

    auto mouse_pos = POINT{};
    ::GetCursorPos(&mouse_pos);
    ::ScreenToClient(hWnd, &mouse_pos);

    if (window->on_mouse_enter) {
      window->on_mouse_enter(window, mouse_pos.x, mouse_pos.y);
    }
  } break;

  case WM_MOUSELEAVE: {
    window->is_mouse_tracking = false;
    auto mouse_pos = POINT{};
    ::GetCursorPos(&mouse_pos);
    ::ScreenToClient(hWnd, &mouse_pos);

    if (window->on_mouse_leave) {
      window->on_mouse_leave(window, mouse_pos.x, mouse_pos.y);
    }
  } break;

  case WM_MOUSEMOVE: {
    if (!window->is_mouse_tracking) {
      TRACKMOUSEEVENT tme;
      tme.cbSize = sizeof(tme);
      tme.hwndTrack = hWnd;
      tme.dwFlags = TME_HOVER | TME_LEAVE;
      tme.dwHoverTime = 0;
      window->is_mouse_tracking = ::_TrackMouseEvent(&tme);
    }

    const auto x = GET_X_LPARAM(lParam);
    const auto y = GET_Y_LPARAM(lParam);
    window->mouse_x = x;
    window->mouse_y = y;
    if (window->on_mouse_move) {
      window->on_mouse_move(window, x, y);
    }
  } break;

    // mouse left button
  case WM_LBUTTONDOWN: {
    const auto x = GET_X_LPARAM(lParam);
    const auto y = GET_Y_LPARAM(lParam);

    if (not window->mouse_states[(int)rugui::MouseButton::Left]) {
      window->mouse_just_states[(int)rugui::MouseButton::Left] = 1;
    }
    window->mouse_states[(int)rugui::MouseButton::Left] = true;

    if (window->on_mouse_down) {
      window->on_mouse_down(window, rugui::MouseButton::Left, x, y);
    }
  } break;
  case WM_LBUTTONUP: {
    const auto x = GET_X_LPARAM(lParam);
    const auto y = GET_Y_LPARAM(lParam);

    if (window->mouse_states[(int)rugui::MouseButton::Left]) {
      window->mouse_just_states[(int)rugui::MouseButton::Left] = 2;
    }
    window->mouse_states[(int)rugui::MouseButton::Left] = false;

    if (window->on_mouse_up) {
      window->on_mouse_up(window, rugui::MouseButton::Left, x, y);
    }
  } break;

    // mouse right button
  case WM_RBUTTONDOWN: {
    const auto x = GET_X_LPARAM(lParam);
    const auto y = GET_Y_LPARAM(lParam);

    if (not window->mouse_states[(int)rugui::MouseButton::Right]) {
      window->mouse_just_states[(int)rugui::MouseButton::Right] = 1;
    }
    window->mouse_states[(int)rugui::MouseButton::Right] = true;

    if (window->on_mouse_down) {
      window->on_mouse_down(window, rugui::MouseButton::Right, x, y);
    }
  } break;
  case WM_RBUTTONUP: {
    const auto x = GET_X_LPARAM(lParam);
    const auto y = GET_Y_LPARAM(lParam);

    if (window->mouse_states[(int)rugui::MouseButton::Right]) {
      window->mouse_just_states[(int)rugui::MouseButton::Right] = 2;
    }
    window->mouse_states[(int)rugui::MouseButton::Right] = false;

    if (window->on_mouse_up) {
      window->on_mouse_up(window, rugui::MouseButton::Right, x, y);
    }
  } break;

    // mouse middle button
  case WM_MBUTTONDOWN: {
    const auto x = GET_X_LPARAM(lParam);
    const auto y = GET_Y_LPARAM(lParam);

    if (not window->mouse_states[(int)rugui::MouseButton::Middle]) {
      window->mouse_just_states[(int)rugui::MouseButton::Middle] = 1;
    }
    window->mouse_states[(int)rugui::MouseButton::Middle] = true;

    if (window->on_mouse_down) {
      window->on_mouse_down(window, rugui::MouseButton::Middle, x, y);
    }
  } break;
  case WM_MBUTTONUP: {
    const auto x = GET_X_LPARAM(lParam);
    const auto y = GET_Y_LPARAM(lParam);

    if (window->mouse_states[(int)rugui::MouseButton::Middle]) {
      window->mouse_just_states[(int)rugui::MouseButton::Middle] = 2;
    }
    window->mouse_states[(int)rugui::MouseButton::Middle] = false;

    if (window->on_mouse_up) {
      window->on_mouse_up(window, rugui::MouseButton::Middle, x, y);
    }
  } break;

    // mouse wheel scroll
  case WM_MOUSEWHEEL: {
    const auto scroll_delta = GET_WHEEL_DELTA_WPARAM(wParam);
    if (window->on_mouse_scroll) {
      window->on_mouse_scroll(window, scroll_delta);
    }
  } break;

    // keyboard
  case WM_KEYDOWN: {
    const auto keycode = wParam;

    if (not window->key_states[keycode]) {
      window->key_just_states[keycode] = 1;
    }
    window->key_states[keycode] = true;

    if (window->on_key_down) {
      window->on_key_down(window, keycode);
    }
  } break;
  case WM_KEYUP: {
    const auto keycode = wParam;

    if (window->key_states[keycode]) {
      window->key_just_states[keycode] = 2;
    }
    window->key_states[keycode] = false;

    if (window->on_key_up) {
      window->on_key_up(window, keycode);
    }
  } break;
  }

  return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

} // namespace ruapp
