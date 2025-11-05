#ifndef SCENE_H
#define SCENE_H

#include <optional>
#include <vector>

#include "geometry/ray.h"
#include "geometry/mesh.h"
#include "geometry/sphere.h"
#include "geometry/square.h"
#include "material.h"


class Scene {
  std::vector<Mesh> meshes;
  std::vector<Sphere> spheres;
  std::vector<Square> squares;
  std::vector<Light> lights;

public:
  void draw() const;

  RayIntersection compute_intersection(const Ray &p_ray) const;
  kmath::Lrgb ray_trace_recursive(const Ray &p_ray, const int p_bounce_count = 4) const;
  kmath::Lrgb ray_trace(const Ray &p_ray_start) const;
  
  void setup_single_sphere();
  void setup_single_square();
  void setup_cornell_box();

public:
  kmath::Lrgb _intersection_get_color(const Ray &p_ray, const RayIntersection &p_intersection) const;
  std::optional<const Material*> _intersection_get_material(const RayIntersection &p_intersection) const;
};



#endif // #ifndef SCENE_H
