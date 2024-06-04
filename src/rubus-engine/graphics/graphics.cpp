#include "graphics.hpp"

#include <array>
#include <format>
#include <iostream>

#include <stb_image.h>
#include <glad/glad.h>

namespace graphics {

auto Mesh::delete_buffers() -> void {
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
}

auto world_to_screen_space(float width, float height, glm::mat4 mvp, glm::vec2 pos) -> glm::vec2 {
  // https://stackoverflow.com/a/57938288
  auto clip_space = mvp * glm::vec4{pos.x, pos.y, 0.f, 1.f};
  auto ndc_space = glm::vec3{clip_space.x, clip_space.y, clip_space.z} / clip_space.w;
  auto screen_space = glm::vec2{(ndc_space.x + 1) * width / 2.f, (1 - ndc_space.y) * height / 2.f};
  return screen_space;
}

auto screen_to_world_space(float width, float height, glm::mat4 vp, glm::vec2 pos) -> glm::vec2 {
  auto x_ndc = (2.0f * pos.x) / width - 1.0f;
  auto y_ndc = 1.0f - (2.0f * pos.y) / height;
  auto inverse_vp = glm::inverse(vp);
  auto clip_space = glm::vec4(x_ndc, y_ndc, 0.0f, 1.0f); // convert from ndc to clip space
  auto world_space = inverse_vp * clip_space; // apply the inverse mvp matrix to convert clip space to world space
  auto world_pos = glm::vec2(world_space) / world_space.w; // perform perspective division to get world coordinates
  return {world_pos.x, world_pos.y};
}

auto compile_shader(int shader_type, std::span<const char *> shader_src) -> uint32_t {
  auto shader = uint32_t{};
  shader = glCreateShader(shader_type);
  glShaderSource(shader, 1, shader_src.data(), nullptr);
  glCompileShader(shader);

  auto success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    auto msg = std::array<char, 512>{};
    glGetShaderInfoLog(shader, msg.size(), nullptr, msg.data());
    glDeleteShader(shader);
    std::cout << std::format("Error: shader compilation failed\n{}\n", msg.data());
  }

  return shader;
}

auto link_shaders(std::initializer_list<uint32_t> shaders) -> uint32_t {
  auto program = uint32_t{};
  program = glCreateProgram();
  for (auto shader : shaders) {
    glAttachShader(program, shader);
  }
  glLinkProgram(program);

  for (auto shader : shaders) {
    glDetachShader(program, shader);
    glDeleteShader(shader);
  }

  auto success = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    auto msg = std::array<char, 512>{};
    glGetProgramInfoLog(program, msg.size(), nullptr, msg.data());
    glDeleteProgram(program);
    std::cout << std::format("Error: shader linking failed\n{}\n", msg.data());
  }

  return program;
}

auto set_uniform_mat4f(uint32_t shader_program, const char *name, float *value_ptr) -> void {
  auto loc = glGetUniformLocation(shader_program, name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr);
}

auto make_quad_mesh(glm::vec3 tr, glm::vec3 tl, glm::vec3 bl, glm::vec3 br) -> Mesh {
  auto vao = uint32_t{};
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // clang-format off
  auto vertices = std::array{
    // posiontion     uv
    tr.x, tr.y, tr.z, 1.0f, 1.0f,
    tl.x, tl.y, tl.z, 0.0f, 1.0f,
    bl.x, bl.y, bl.z, 0.0f, 0.0f,
    br.x, br.y, br.z, 1.0f, 0.0f,
  };
  auto indices = std::array<uint8_t, 6>{
    0, 1, 3,
    1, 2, 3,
  };
  // clang-format on

  constexpr auto stride = 5 * sizeof(float);

  auto vbo = uint32_t{};
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

  // positions
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0); // NOLINT

  // uv
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void *)(3 * sizeof(float))); // NOLINT

  auto ebo = uint32_t{};
  glGenBuffers(1, &ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint8_t), indices.data(), GL_STATIC_DRAW);

  // reset state
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  return {vao, vbo, ebo};
}

auto make_quad_mesh(glm::vec2 pivot, float width, float height) -> Mesh {
  auto tr = glm::vec3{+width * pivot.x, +height * pivot.y, 0};
  auto tl = glm::vec3{-width * pivot.x, +height * pivot.y, 0};
  auto bl = glm::vec3{-width * pivot.x, -height * pivot.y, 0};
  auto br = glm::vec3{+width * pivot.x, -height * pivot.y, 0};
  return make_quad_mesh(tr, tl, bl, br);
}

auto draw_quad(uint32_t vao) -> void {
  glBindVertexArray(vao);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
  glBindVertexArray(0);
}

} // namespace graphics
