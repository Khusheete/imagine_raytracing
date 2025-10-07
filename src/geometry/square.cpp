#include "square.h"

#include "kmath/vector.hpp"
#include "plane.h"

#include "kmath/euclidian_flat_3d.hpp"
#include <optional>
#include <pthread.h>


using namespace kmath;


void Square::set_quad(const Vec3 &p_bottom_left, const Vec3 &p_right_vector, const Vec3 &p_up_vector, const Vec2 &p_size, const Vec2 &p_uv_min, const Vec2 &p_uv_max) {
  right_vector = normalized(p_right_vector);
  up_vector = normalized(p_up_vector);
  normal = normalized(cross(p_right_vector, p_up_vector));
  bottom_left = p_bottom_left;
  size = p_size;

  const Vec3 right_displacement = right_vector * p_size.x;
  const Vec3 up_displacement = up_vector * p_size.x;

  vertices.clear();
  vertices.resize(4);

  vertices[0].position = p_bottom_left;
  vertices[1].position = p_bottom_left + right_displacement;
  vertices[2].position = p_bottom_left + right_displacement + up_displacement;
  vertices[3].position = p_bottom_left + up_displacement;

  vertices[0].uv = Vec2(p_uv_min.x, p_uv_max.x);
  vertices[1].uv = Vec2(p_uv_max.y, p_uv_max.x);
  vertices[2].uv = Vec2(p_uv_max.y, p_uv_max.y);
  vertices[3].uv = Vec2(p_uv_min.x, p_uv_max.y);

  vertices[0].normal = vertices[1].normal = vertices[2].normal = vertices[3].normal = normal;

  triangles.clear();
  triangles.resize(2);
  triangles[0][0] = 0;
  triangles[0][1] = 1;
  triangles[0][2] = 2;
  triangles[1][0] = 0;
  triangles[1][1] = 2;
  triangles[1][2] = 3;
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


void Square::translate(const kmath::Vec3 &p_translation) {
  bottom_left += p_translation;
  Mesh::translate(p_translation);
}


void Square::apply_transformation_matrix(const kmath::Mat3 &p_transform) {
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

  Mesh::apply_transformation_matrix(p_transform);
}


Square::Square(): Mesh() {}


Square::Square(const Vec3 &p_bottom_left, const Vec3 &p_right_vector, const Vec3 &p_up_vector, const Vec2 &p_size, const Vec2 &p_uv_min, const Vec2 &p_uv_max) : Mesh() {
  set_quad(p_bottom_left, p_right_vector, p_up_vector, p_size, p_uv_min, p_uv_max);
}
