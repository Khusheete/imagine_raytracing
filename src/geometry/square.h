#ifndef SQUARE_H
#define SQUARE_H

#include "mesh.h"

#include "kmath/vector.hpp"

#include <cmath>


class Square : public Mesh {
public:
  kmath::Vec3 m_normal;
  kmath::Vec3 m_bottom_left;
  kmath::Vec3 m_right_vector;
  kmath::Vec3 m_up_vector;

  void set_quad(const kmath::Vec3 &p_bottom_left, const kmath::Vec3 &p_right_vector, const kmath::Vec3 &p_up_vector, const kmath::Vec2 &p_size = kmath::Vec2::ONE, const kmath::Vec2 &p_uv_min = kmath::Vec2::ZERO, const kmath::Vec2 &p_uv_max = kmath::Vec2::ZERO);
  RaySquareIntersection intersect(const Ray &p_ray) const;

  Square();
  Square(const kmath::Vec3 &p_bottom_left, const kmath::Vec3 &p_right_vector, const kmath::Vec3 &p_up_vector, const kmath::Vec2 &p_size = kmath::Vec2::ONE, const kmath::Vec2 &p_uv_min = kmath::Vec2::ZERO, const kmath::Vec2 &p_uv_max = kmath::Vec2::ZERO);
  virtual ~Square() = default;
};


#endif // SQUARE_H
