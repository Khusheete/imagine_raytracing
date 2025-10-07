#ifndef PLANE_H
#define PLANE_H

#include <optional>

#include "kmath/euclidian_flat_3d.hpp"
#include "kmath/vector.hpp"
#include "ray.h"


kmath::Vec3 project(const kmath::Vec3 &p_point, const kmath::Plane3 &p_plane);

float distance(const kmath::Vec3 &p_point, const kmath::Plane3 &p_plane);

std::optional<kmath::Vec3> get_intersection(const Ray &p_ray, const kmath::Plane3 &p_plane);
bool are_parallel(const Ray &p_ray, const kmath::Plane3 &p_plane);


#endif // #ifndef PLANE_H
