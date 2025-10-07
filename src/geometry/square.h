#ifndef SQUARE_H
#define SQUARE_H

#include "mesh.h"

#include "kmath/vector.hpp"

#include <cmath>


class Square : public Mesh {
public:
  void set_quad(const kmath::Vec3 &p_bottom_left, const kmath::Vec3 &p_right_vector, const kmath::Vec3 &p_up_vector, const kmath::Vec2 &p_size = kmath::Vec2::ONE, const kmath::Vec2 &p_uv_min = kmath::Vec2::ZERO, const kmath::Vec2 &p_uv_max = kmath::Vec2::ZERO);
  RaySquareIntersection intersect(const Ray &p_ray) const;

  virtual void translate(const kmath::Vec3 &p_translation) override;
  virtual void apply_transformation_matrix(const kmath::Mat3 &p_transform) override;

  Square();
  Square(const kmath::Vec3 &p_bottom_left, const kmath::Vec3 &p_right_vector, const kmath::Vec3 &p_up_vector, const kmath::Vec2 &p_size = kmath::Vec2::ONE, const kmath::Vec2 &p_uv_min = kmath::Vec2::ZERO, const kmath::Vec2 &p_uv_max = kmath::Vec2::ZERO);
  virtual ~Square() = default;

private:
  kmath::Vec3 normal;
  kmath::Vec3 bottom_left;
  kmath::Vec3 right_vector;
  kmath::Vec3 up_vector;
  kmath::Vec2 size;
};


#endif // SQUARE_H
