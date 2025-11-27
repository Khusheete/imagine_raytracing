#version 440 core


#section vertex


layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;
layout(location = 2) in vec2 i_uv;


uniform mat4 u_projection_view;
uniform mat4 u_model;


layout(location = 0) out vec3 o_position;
layout(location = 1) out vec3 o_normal;
layout(location = 2) out vec2 o_uv;


void main() {
    vec4 world_position = u_model * vec4(i_position, 1.0);
    o_position = world_position.xyz;
    gl_Position = u_projection_view * world_position;

    const mat3 normal_transform = transpose(inverse(mat3(u_model)));
    o_normal = normal_transform * i_normal;

    o_uv = i_uv;
}


#section fragment


layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_normal;
layout(location = 2) in vec2 i_uv;


layout(location = 0) out vec4 o_color;

uniform vec3 u_color;


void main() {
    // o_color = vec4(i_position, 1.0);
    o_color = vec4(u_color, 1.0);
}
