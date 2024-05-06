add_library(glm INTERFACE "")

target_sources(
  glm
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS
      vendor/glm
)
