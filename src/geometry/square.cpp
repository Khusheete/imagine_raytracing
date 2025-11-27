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


#include "square.hpp"

#include "thirdparty/kmath/matrix.hpp"
#include "thirdparty/kmath/vector.hpp"
#include "thirdparty/kmath/euclidian_flat_3d.hpp"

#include "plane.hpp"

#include <optional>


using namespace kmath;


void Square::set_quad(const Vec3 &p_bottom_left, const Vec3 &p_right_vector, const Vec3 &p_up_vector, const Vec2 &p_size, const Vec2 &p_uv_min, const Vec2 &p_uv_max) {
  right_vector = normalized(p_right_vector);
  up_vector = normalized(p_up_vector);
  normal = normalized(cross(p_right_vector, p_up_vector));
  bottom_left = p_bottom_left;
  size = p_size;
  uv_min = p_uv_min;
  uv_max = p_uv_max;
}


RaySquareIntersection Square::intersect(const Ray &p_ray) const {
  RaySquareIntersection intersection;
  const Plane3 plane = Plane3::plane(normal, dot(normal, bottom_left));
  const std::optional<Vec3> intersection_point_opt = get_intersection(p_ray, plane);
  if (!intersection_point_opt.has_value()) return intersection;

  const Vec3 &intersection_point = intersection_point_opt.value();
  const Vec3 local_intersection = intersection_point - bottom_left;

  const Vec2 uv = Vec2(
    dot(right_vector, local_intersection),
    dot(up_vector, local_intersection)
  ) / size;

  if (uv.x < 0.0f || uv.x > 1.0f || uv.y < 0.0f || uv.y > 1.0f) return intersection;
  
  intersection.exists = true;
  intersection.position = intersection_point;
  intersection.distance = distance(p_ray.origin, intersection_point);
  intersection.normal = normal;
  intersection.uv = uv;
  
  return intersection;
}


void Square::translate(const Vec3 &p_translation) {
  bottom_left += p_translation;
}


void Square::scale(const Vec3 &p_scale) {
  apply_transformation_matrix(Mat3::scale(p_scale));
}


void Square::rotate_x(const float p_angle_degrees) {
  float angle = p_angle_degrees * M_PI / 180.;
  apply_transformation_matrix(Mat3::x_rotation(angle));
}


void Square::rotate_y(const float p_angle_degrees) {
  float angle = p_angle_degrees * M_PI / 180.;
  apply_transformation_matrix(Mat3::y_rotation(angle));
}


void Square::rotate_z(const float p_angle_degrees) {
  float angle = p_angle_degrees * M_PI / 180.;
  apply_transformation_matrix(Mat3::z_rotation(angle));
}


void Square::apply_transformation_matrix(const Mat3 &p_transform) {
  Vec3 position = bottom_left + 0.5f * (size.x * right_vector + size.y * up_vector);
  position = p_transform * position;
  
  right_vector = p_transform * right_vector;
  up_vector = p_transform * up_vector;
  normal = normalized(cross(right_vector, up_vector));

  const float right_length = length(right_vector);
  size.x *= right_length;
  right_vector /= right_length;

  const float up_length = length(up_vector);
  size.y *= up_length;
  up_vector /= up_length;

  bottom_left = position - 0.5f * (size.x * right_vector + size.y * up_vector);
}


Square::Square() {}


Square::Square(const Vec3 &p_bottom_left, const Vec3 &p_right_vector, const Vec3 &p_up_vector, const Vec2 &p_size, const Vec2 &p_uv_min, const Vec2 &p_uv_max) {
  set_quad(p_bottom_left, p_right_vector, p_up_vector, p_size, p_uv_min, p_uv_max);
}
