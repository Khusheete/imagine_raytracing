#pragma once


#include "kmath/vector.hpp"
#include "kmath/color.hpp"


// enum class LightType : bool {
//   SPHERICAL,
//   // QUAD,
// };


struct Light {
  kmath::Vec3 pos;
  kmath::Lrgb color;
  float energy;
  float radius;
  float power_correction;
};



