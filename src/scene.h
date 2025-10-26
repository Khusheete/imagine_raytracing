#ifndef SCENE_H
#define SCENE_H

#include <vector>

#include "kmath/vector.hpp"

#include "geometry/ray.h"
#include "geometry/mesh.h"
#include "geometry/sphere.h"
#include "geometry/square.h"


class Scene {
  std::vector<Mesh> meshes;
  std::vector<Sphere> spheres;
  std::vector<Square> squares;
  std::vector<Light> lights;

public:
  void draw() const;

  RayIntersection compute_intersection(const Ray &p_ray) const;
  kmath::Vec3 ray_trace_recursive(const Ray &p_ray, const int p_remaining_bounces = 10) const;
  kmath::Vec3 ray_trace(const Ray &p_ray_start) const;
  
  void setup_single_sphere();
  void setup_single_square();
  void setup_cornell_box();
};



#endif // #ifndef SCENE_H
