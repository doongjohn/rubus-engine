add_library(stb STATIC "")

set_property(TARGET stb PROPERTY MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)

target_sources(
  stb
  PRIVATE
    vendor/stb/stb_image.c
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      vendor/stb
    FILES
      vendor/stb/stb_image.h
)
