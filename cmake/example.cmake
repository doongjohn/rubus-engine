add_executable(rubus-engine-example-game WIN32 "")

set_property(TARGET rubus-engine-example-game PROPERTY EXCLUDE_FROM_ALL true)
set_property(TARGET rubus-engine-example-game PROPERTY CXX_STANDARD 20)

set(USE_ASAN false CACHE BOOL "Use AddressSanitizer")
if (USE_ASAN)
  set(CMAKE_CXX_FLAGS_DEBUG "-O1")
  set_property(TARGET rubus-engine-example-game PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded)
  set(SANITIZER_OPTS -fno-omit-frame-pointer -fno-sanitize-recover=all -fsanitize=address,undefined)
  target_compile_options(rubus-engine-example-game PRIVATE ${SANITIZER_OPTS})
  target_link_options(rubus-engine-example-game PRIVATE ${SANITIZER_OPTS})
else()
  set_property(TARGET rubus-engine-example-game PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)
endif()

target_sources(
  rubus-engine-example-game
  PRIVATE
    example/main.cpp
)

target_compile_options(
  rubus-engine-example-game
  PRIVATE
    -Wall
    -Wextra
)

target_link_libraries(
  rubus-engine-example-game
  PRIVATE
    rubus-engine
)

skia_copy_icudtl_dat(rubus-engine-example-game)
