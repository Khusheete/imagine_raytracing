#pragma once


#include "kmath/rotor_3d.hpp"
#include "kmath/vector.hpp"


struct FreeCamera {
  kmath::Vec3 position;
  kmath::Rotor3 rotation = kmath::Rotor3::IDENTITY;


public:
  void local_translate(const kmath::Vec3 &p_delta);
  void local_rotate(const kmath::Vec2 &p_delta);
  
  kmath::Mat4 get_view_matrix() const;
  kmath::Mat4 get_inv_view_matrix() const;
};

