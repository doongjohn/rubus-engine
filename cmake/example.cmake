add_executable(rubus-engine-example-game WIN32 "")

set_property(TARGET rubus-engine-example-game PROPERTY EXCLUDE_FROM_ALL true)
set_property(TARGET rubus-engine-example-game PROPERTY CXX_STANDARD 20)
set_property(TARGET rubus-engine-example-game PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)

target_sources(
  rubus-engine-example-game
  PRIVATE
    example/main.cpp
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(
    rubus-engine-example-game
    PRIVATE
      -Wall
      -Wextra
  )
endif()

target_link_libraries(
  rubus-engine-example-game
  PRIVATE
    rubus-engine
)

skia_copy_icudtl_dat(rubus-engine-example-game)
