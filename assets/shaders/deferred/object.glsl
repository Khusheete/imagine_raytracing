#version 440 core


#section vertex


layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;
layout(location = 2) in vec2 i_uv;

uniform mat4 u_model;
uniform mat4 u_projection_view;

layout(location = 0) out vec4 o_position;
layout(location = 1) out vec3 o_normal;
layout(location = 2) out vec2 o_uv;


void main() {
  o_position = u_model * vec4(i_position, 1.0);
  gl_Position = u_projection_view * o_position;
  const mat3 normal_transform = transpose(inverse(mat3(u_model)));
  o_normal = normalize(normal_transform * i_normal);
  o_uv = i_uv;
}


#section fragment


layout(location = 0) in vec4 i_position;
layout(location = 1) in vec3 i_normal;
layout(location = 2) in vec2 i_uv;

layout(location = 0) out vec4 o_position;
layout(location = 1) out vec4 o_color;
layout(location = 2) out vec4 o_normal;


void main() {
  o_position = i_position;
  o_color = vec4(1.0);
  o_normal = vec4(normalize(i_normal), 1.0f);
}

