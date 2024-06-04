#pragma once

#include <cstdint>
#include <span>
#include <initializer_list>

#include <glad/glad.h>
#include <glm/glm.hpp>

namespace graphics {

struct Mesh {
  uint32_t vao = 0;
  uint32_t vbo = 0;
  uint32_t ebo = 0;

  auto delete_buffers() -> void;
};

auto world_to_screen_space(float width, float height, glm::mat4 mvp, glm::vec2 pos = {0, 0}) -> glm::vec2;
auto screen_to_world_space(float width, float height, glm::mat4 mvp, glm::vec2 pos) -> glm::vec2;

auto compile_shader(int shader_type, std::span<const char *> shader_src) -> uint32_t;
auto link_shaders(std::initializer_list<uint32_t> shaders) -> uint32_t;

auto set_uniform_mat4f(uint32_t shader_program, const char *name, float *value_ptr) -> void;

auto make_quad_mesh(glm::vec3 tr, glm::vec3 tl, glm::vec3 bl, glm::vec3 br) -> Mesh;
auto make_quad_mesh(glm::vec2 pivot, float width, float height) -> Mesh;

auto draw_quad(uint32_t vao) -> void;

} // namespace graphics
