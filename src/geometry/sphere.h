#ifndef Sphere_H
#define Sphere_H

#include "kmath/vector.hpp"
#include "mesh.h"
#include <cmath>



class Sphere : public Mesh {
public:
  kmath::Vec3 center;
  float radius;

  virtual void build_arrays() override;
  virtual void translate(const kmath::Vec3 &p_translation) override;
  virtual void apply_transformation_matrix(const kmath::Mat3 &p_transform) override;
  RaySphereIntersection intersect(const Ray &p_ray) const;

  Sphere();
  Sphere(const kmath::Vec3 &p_center, const float r);
  virtual ~Sphere() = default;
};


#endif
