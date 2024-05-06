#version 460 core

layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_uv;

uniform mat4 mvp;

out vec2 uv;

void main() {
    gl_Position = mvp * vec4(in_position, 1);
    uv = in_uv;
}
