#include "material.h"


using namespace kmath;


Lrgb Material::get_light_influence(const Vec3 &p_fragment_position, const Vec3 &p_surface_normal, const Vec3 &p_camera_direction, const Light &p_light) const {
  const Vec3 light_direction = normalized(p_light.pos - p_fragment_position);
  
  const float signed_light_direction = dot(p_surface_normal, light_direction);

  if (signed_light_direction > 0.0) {
    const float light_distance = distance(p_light.pos, p_fragment_position);
    const float light_attenuation = std::pow(1.0 / light_distance, p_light.power_correction);
    
    const float diffuse_contrib = std::max(0.0f, signed_light_direction);
    const Vec3 diffuse = light_attenuation * p_light.energy * diffuse_contrib * p_light.color;

    const Vec3 reflected_dir = 2.0f * signed_light_direction * p_surface_normal - light_direction;
    const float specular_contrib = std::pow(std::max(0.0f, dot(p_camera_direction, reflected_dir)), shininess);
    const Vec3 specular = light_attenuation * p_light.energy * specular_contrib * specular_material * p_light.color;

    return diffuse * diffuse_material + specular * specular_material;
  }

  return Lrgb::ZERO;
}
