#include "plane.h"

#include "kmath/euclidian_flat_3d.hpp"

using namespace kmath;


Vec3 project(const Vec3 &p_point, const Plane3 &p_plane) {
  return as_vector(
      fast_project(Point3::point(p_point), p_plane)
  );
}


float distance(const Vec3 &p_point, const Plane3 &p_plane) {
  return std::abs(meet(Point3::point(p_point), p_plane));
}


kmath::Vec3 get_intersection(const Ray &p_ray, const kmath::Plane3 &p_plane) {
  const Line3 plucker = Line3::line(p_ray.origin, p_ray.direction);
  const Point3 inter = meet(plucker, p_plane);
  return kmath::as_vector(inter);
}


bool are_parallel(const Ray &p_ray, const kmath::Plane3 &p_plane) {
  const Line3 plucker = Line3::line(p_ray.origin, p_ray.direction);
  const Point3 inter = meet(plucker, p_plane);
  return is_vanishing(inter);
}
