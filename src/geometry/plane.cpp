/* ------------------------------------------------------------------------------------------------------------------ *
*                                                                                                                     *
*                                                                                                                     *
*                                                /\                    ^__                                            *
*                                               /#*\  /\              /##@>                                           *
*                                              <#* *> \/         _^_  \\    _^_                                       *
*                                               \##/            /###\ \è\  /###\                                      *
*                                                \/ /\         /#####n/xx\n#####\                                     *
*                   Ferdinand                       \/         \###^##xXXx##^###/                                     *
*                        Souchet                                \#/ V¨\xx/¨V \#/                                      *
*                     (aka. @Khusheete)                          V     \c\    V                                       *
*                                                                       //                                            *
*                                                                     \o/                                             *
*             ferdinand.souchet@etu.umontpellier.fr                    v                                              *
*                                                                                                                     *
*                                                                                                                     *
*                                                                                                                     *
*                                                                                                                     *
* Copyright 2025 Ferdinand Souchet (aka. @Khusheete)                                                                  *
*                                                                                                                     *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated        *
* documentation files (the “Software”), to deal in the Software without restriction, including without limitation the *
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to     *
* permit persons to whom the Software is furnished to do so, subject to the following conditions:                     *
*                                                                                                                     *
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of    *
* the Software.                                                                                                       *
*                                                                                                                     *
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO    *
* THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, *
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE      *
* SOFTWARE.                                                                                                           *
*                                                                                                                     *
* ------------------------------------------------------------------------------------------------------------------ */


#include "plane.hpp"

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
  // The intersection point of the ray and the plane is the meet (outer product) of
  // the line and the plane (in 3D PGA). It is the trivector representing the
  // bundle (subspace) of planes that are contained both in `p_plane`, and in `line`.
  const Point3 inter = meet(line, p_plane);
  if (inter.e123 > -0.001) {
    // The projective part of the intersection point must be negative (ie. the ray is pointing towards the plane),
    // and not too close to zero (ie. the ray is parallel to the plane)
    return std::optional<Vec3>();
  } else {
    const Vec3 intersection_point = as_vector(inter);
    const Vec3 direction = intersection_point - p_ray.origin;
    if (dot(direction, p_ray.direction) < 0.0f) { // Don't return anything if the quad is behind the ray origin.
      return std::optional<Vec3>();
    }
    return intersection_point;
  }
}


bool are_parallel(const Ray &p_ray, const Plane3 &p_plane) {
  const Line3 plucker = Line3::line(p_ray.origin, p_ray.direction);
  const Point3 inter = meet(plucker, p_plane);
  return is_vanishing(inter);
}
