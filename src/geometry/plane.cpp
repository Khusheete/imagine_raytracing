#include "plane.h"

using namespace kmath;


Vec3 project(const Vec3 &p_point, const Plane3 &p_plane) {
  return as_vector(
    fast_project(Point3::point(p_point), p_plane)
  );
}


float distance(const Vec3 &p_point, const Plane3 &p_plane) {
  return std::abs(meet(Point3::point(p_point), p_plane));
}


std::optional<Vec3> get_intersection(const Ray &p_ray, const Plane3 &p_plane) {
  const Line3 line = Line3::line(p_ray.direction, p_ray.origin);
  const Point3 inter = meet(line, p_plane);
  if (inter.e123 > -0.001) {
    // The projective part of the intersection point must be negative (ie. the ray is pointing towards the plane),
    // and not too close to zero (ie. the ray is parallel to the plane)
    return std::optional<Vec3>();
  } else {
    return std::optional<Vec3>(as_vector(inter));
  }
}


bool are_parallel(const Ray &p_ray, const Plane3 &p_plane) {
  const Line3 plucker = Line3::line(p_ray.origin, p_ray.direction);
  const Point3 inter = meet(plucker, p_plane);
  return is_vanishing(inter);
}
