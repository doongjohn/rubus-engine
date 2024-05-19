cmake_minimum_required(VERSION 3.27)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)

project(
  RubusEngine
  VERSION 0.1.0
  DESCRIPTION ""
  HOMEPAGE_URL ""
)

if (WIN32 AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "-fdiagnostics-color=always -fansi-escape-codes")
endif()

if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(FATAL "This project only supports clang")
endif()

add_library(rubus-engine "")

set_property(TARGET rubus-engine PROPERTY CXX_STANDARD 20)

set(USE_ASAN false CACHE BOOL "Use AddressSanitizer")
if (USE_ASAN)
  set(CMAKE_CXX_FLAGS_DEBUG "-O1")
  set_property(TARGET rubus-engine PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
  set(SANITIZER_OPTS -fno-omit-frame-pointer -fno-sanitize-recover=all -fsanitize=address,undefined)
  target_compile_options(rubus-engine PRIVATE ${SANITIZER_OPTS})
  target_link_options(rubus-engine PRIVATE ${SANITIZER_OPTS})
else()
  set_property(TARGET rubus-engine PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)
endif()

target_sources(
  rubus-engine
  PRIVATE
    src/rubus-engine/utils/utils.cpp
    src/rubus-engine/app/app.cpp
    src/rubus-engine/graphics/graphics.cpp
    src/rubus-engine/game/resource.cpp
    src/rubus-engine/game/game.cpp
    src/rubus-engine/game/game_object.cpp
    src/rubus-engine/game/scene.cpp
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      src
    FILES
      src/rubus-engine/utils/utils.hpp
      src/rubus-engine/app/wglext.h
      src/rubus-engine/app/app.hpp
      src/rubus-engine/graphics/graphics.hpp
      src/rubus-engine/game/resource.hpp
      src/rubus-engine/game/game.hpp
      src/rubus-engine/game/game_object.hpp
      src/rubus-engine/game/scene.hpp
)

target_compile_options(
  rubus-engine
  PRIVATE
    -Wall
    -Wextra
)

target_link_libraries(
  rubus-engine
  PUBLIC
    comctl32
    stb
    glm
    rubus-gui
)

include("cmake/stb.cmake")
include("cmake/glm.cmake")
include("cmake/rubus-gui.cmake")
include("cmake/example.cmake")
