#include "material.h"


Material::Material() {
  type = MaterialType::DIFFUSE_BLINN_PHONG;
  transparency = 0.0f;
  index_medium = 1.0f;
  ambient_material = kmath::Vec3(0.0f, 0.0f, 0.0f);
}
