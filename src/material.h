#ifndef MATERIAL_H
#define MATERIAL_H

#include "kmath/vector.hpp"

#include <cmath>


enum class MaterialType : unsigned char {
  DIFFUSE_BLINN_PHONG,
  GLASS,
  MIRROR
};


struct Material {
  kmath::Vec3 ambient_material;
  kmath::Vec3 diffuse_material;
  kmath::Vec3 specular_material;
  double shininess;

  float index_medium;
  float transparency;

  MaterialType type;

  Material();
};



#endif // MATERIAL_H
