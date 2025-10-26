#ifndef MATERIAL_H
#define MATERIAL_H

#include "kmath/vector.hpp"
#include "kmath/color.hpp"
#include "light.hpp"

#include <cmath>
#include <iterator>


enum class MaterialType : unsigned char {
  DIFFUSE_BLINN_PHONG,
  GLASS,
  MIRROR
};


struct Material {
public:
  kmath::Lrgb ambient_material = kmath::Lrgb::ZERO;
  kmath::Lrgb diffuse_material = kmath::Lrgb::ONE;
  kmath::Lrgb specular_material = kmath::Lrgb::ZERO;
  double shininess = 1.0;

  float index_medium = 0.0f;
  float transparency = 0.0f;

  MaterialType type = MaterialType::DIFFUSE_BLINN_PHONG;

public:
  template<typename LightIt>
  kmath::Lrgb get_color(const kmath::Vec3 &p_fragment_position, const kmath::Vec3 &p_surface_normal, const kmath::Vec3 &p_camera_direction, const kmath::Lrgb &p_ambiant_energy, const LightIt &p_lights) const {
    using namespace kmath;
  
    const Lrgb ambiant = ambient_material * p_ambiant_energy;

    Lrgb diffuse  = Lrgb::ZERO;
    Lrgb specular = Lrgb::ZERO;

    for (const Light &light : p_lights) {
      const Vec3 light_direction = normalized(light.pos - p_fragment_position);
      
      const float signed_light_direction = dot(p_surface_normal, light_direction);

      if (signed_light_direction > 0.0) {
        const float light_distance = distance(light.pos, p_fragment_position);
        const float light_attenuation = std::pow(1.0 / light_distance, light.power_correction);
        
        const float diffuse_contrib = std::max(0.0f, signed_light_direction);
        diffuse += light_attenuation * light.energy * diffuse_contrib * light.color;

        const Vec3 reflected_dir = 2.0f * signed_light_direction * p_surface_normal - light_direction;
        const float specular_contrib = std::pow(std::max(0.0f, dot(p_camera_direction, reflected_dir)), shininess);
        specular += light_attenuation * light.energy * specular_contrib * specular_material * light.color;
      }
    }

    return ambiant + diffuse * diffuse_material + specular * specular_material;
  }
};




#endif // MATERIAL_H
