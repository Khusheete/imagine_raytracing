#ifndef MATERIAL_H
#define MATERIAL_H

#include "kmath/vector.hpp"
#include "kmath/color.hpp"
#include "light.hpp"

#include <cmath>


enum class MaterialType : unsigned char {
  DIFFUSE_BLINN_PHONG,
  GLASS,
  MIRROR
};


struct Material {
public:
  kmath::Lrgb diffuse_material = kmath::Lrgb::ONE;
  kmath::Lrgb specular_material = kmath::Lrgb::ZERO;
  double shininess = 1.0;

  float index_medium = 0.0f;
  float transparency = 0.0f;

  MaterialType type = MaterialType::DIFFUSE_BLINN_PHONG;

public:
  kmath::Lrgb get_light_influence(const kmath::Vec3 &p_fragment_position, const kmath::Vec3 &p_surface_normal, const kmath::Vec3 &p_camera_direction, const Light &p_light) const;
  
  template<typename LightIt>
  kmath::Lrgb get_color(const kmath::Vec3 &p_fragment_position, const kmath::Vec3 &p_surface_normal, const kmath::Vec3 &p_camera_direction, const kmath::Lrgb &p_ambiant_energy, const LightIt &p_lights) const {
    using namespace kmath;
  
    const Lrgb ambiant = diffuse_material * p_ambiant_energy;

    Lrgb light_contribs = Lrgb::ZERO;
    for (const Light &light : p_lights) {
      light_contribs += get_light_influence(p_fragment_position, p_surface_normal, p_camera_direction, light);
    }

    return ambiant + light_contribs;
  }
};




#endif // MATERIAL_H
