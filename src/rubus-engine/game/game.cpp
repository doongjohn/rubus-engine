#include "game.hpp"

#include <array>

#include <glm/ext.hpp>

#include <rubus-engine/utils/utils.hpp>
#include <rubus-engine/graphics/graphics.hpp>
#include "resource.hpp"

namespace rugame {

Camera2d::Camera2d(Screen *screen, glm::vec3 position) : screen{screen}, position{position} {}

auto Camera2d::update() -> void {
  auto screen_left = -screen->width / 2.f;
  auto screen_right = +screen->width / 2.f;
  auto screen_bottom = -screen->height / 2.f;
  auto screen_top = +screen->height / 2.f;
  projection = glm::ortho(screen_left, screen_right, screen_bottom, screen_top, 0.1f, 100.0f);

  auto camera_target = position - glm::vec3{0.f, 0.f, 1.f};
  view = glm::lookAt(position, camera_target, {0.f, 1.f, 0.f});
}

auto Camera2d::world_to_screen_space(glm::mat4 world_transform) -> glm::vec2 {
  return graphics::world_to_screen_space(screen->width, screen->height, projection * view * world_transform);
}

auto Camera2d::screen_to_world_space(glm::vec2 screen_pos) -> glm::vec2 {
  return graphics::screen_to_world_space(screen->width, screen->height, projection * view, screen_pos);
}

SpriteMaterial::SpriteMaterial(std::string texture) : texture{std::move(texture)} {}

auto SpriteMaterial::init() -> void {
  auto vert_shader_str = utils::read_file("shaders/sprite/vert.glsl");
  auto frag_shader_str = utils::read_file("shaders/sprite/frag.glsl");

  auto vert_shader_src = std::array{vert_shader_str.c_str()};
  auto frag_shader_src = std::array{frag_shader_str.c_str()};

  auto vert_shader = graphics::compile_shader(GL_VERTEX_SHADER, vert_shader_src);
  auto frag_shader = graphics::compile_shader(GL_FRAGMENT_SHADER, frag_shader_src);

  shader = graphics::link_shaders({vert_shader, frag_shader});
}

auto SpriteMaterial::deinit() -> void {
  glDeleteProgram(shader);
}

auto SpriteMaterial::bind() -> void {
  auto texture_res = ResourceManager::texture2d.at(texture);
  glUseProgram(shader);
  glBindTexture(GL_TEXTURE_2D, texture_res.handle);
}

auto SpriteMaterial::unbind() -> void {
  glUseProgram(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

Sprite::Sprite(glm::vec2 pivot, float width, float height, SpriteMaterial material) : material{std::move(material)} {
  mesh = graphics::make_quad_mesh(pivot, width, height);
}

Sprite::~Sprite() {
  mesh.delete_buffers();
}

auto Sprite::draw(Camera2d *camera) -> void {
  material.bind();
  auto mvp = camera->projection * camera->view * transform;
  graphics::set_uniform_mat4f(material.shader, "mvp", glm::value_ptr(mvp));
  graphics::draw_quad(mesh.vao);
  material.unbind();
}

} // namespace rugame
