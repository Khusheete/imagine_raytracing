#version 440 core


#section vertex


layout(location = 0) in vec2 i_position;
layout(location = 1) in vec2 i_uv;

layout(location = 0) out vec2 o_uv;


void main() {
  gl_Position = vec4(i_position, 0.0, 1.0f);
  o_uv = i_uv;
}


#section fragment


uniform sampler2D u_position;
uniform sampler2D u_color;
uniform sampler2D u_normal;


struct PointLight {
  // Separate vector components for optimal struct size
  float x, y, z;
  float r, g, b;
  float energy;
  float radius;
};

layout(std430, binding = 0) readonly buffer PointLightsData {
  PointLight point_lights_data[];
};


uniform vec3 u_camera_position;


layout(location = 0) in vec2 i_uv;

layout(location = 0) out vec4 o_color;


void main() {
  const vec3 position = texture(u_position, i_uv).xyz;
  const vec3 albedo = texture(u_color, i_uv).rgb;
  const vec3 normal = texture(u_normal, i_uv).xyz;

  const vec3 view_direction = position - u_camera_position;

  vec3 color = vec3(0.0);// 0.1 * albedo;

  for (int i = 0; i < point_lights_data.length(); i++) {
    const PointLight light = point_lights_data[i];
    const vec3 light_color = vec3(light.r, light.g, light.b);
    const vec3 light_position = vec3(light.x, light.y, light.z);

    vec3 light_direction = light_position - position;
    const float light_distance = length(light_direction);
    light_direction = normalize(light_direction);

    const vec3 reflection_direction = reflect(light_direction, normal);

    const float light_influence = max(light.radius - light_distance, 0.0) / light.radius;

    const float diffuse_energy = max(dot(normal, light_direction), 0.0);
    const float specular_energy = max(dot(reflection_direction, view_direction), 0.0);

    if (diffuse_energy >= 0.0) {
      color += light.energy * (diffuse_energy) * light_color * albedo;
    }
  }

  o_color.rgb = color;
  o_color.a = 1.0;
}
