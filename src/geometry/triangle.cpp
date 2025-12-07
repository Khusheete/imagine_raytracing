#include "triangle.hpp"

#include <optional>

#include "geometry/plane.hpp"
#include "geometry/ray.hpp"
#include "thirdparty/kmath/euclidian_flat_3d.hpp"
#include "thirdparty/kmath/vector.hpp"


using namespace kmath;


Vec2 get_coordinates(const kmath::Vec3 &p_point, const kmath::Vec3 &p_a, const kmath::Vec3 &p_b) {
  const float ab = dot(p_a, p_b);
  const float a2 = length_squared(p_a);
  const float b2 = length_squared(p_b);
  const float pa = dot(p_point, p_a);
  const float pb = dot(p_point, p_b);
  const float fact = 1.0f / (a2 * b2 - ab * ab);
  const float l1 = (b2 * pa - pb * ab) * fact;
  const float l2 = (a2 * pb - pa * ab) * fact;
  return Vec2(l1, l2);
}


std::optional<RayTriangleIntersection> get_intersection(const Ray &p_ray, const Triangle &p_triangle) {
  const Vec3 a = p_triangle.points[1] - p_triangle.points[0];
  const Vec3 b = p_triangle.points[2] - p_triangle.points[0];
  const Vec3 normal = cross(a, b);

  const Plane3 plane = Plane3::plane(normal, 0.0);
  const Ray ray = Ray(p_ray.origin - p_triangle.points[0], p_ray.direction);

  const std::optional<Vec3> plane_intersection = get_intersection(ray, plane);
  if (!plane_intersection.has_value()) return std::optional<RayTriangleIntersection>();

  const Vec3 local_intersection = plane_intersection.value();

  const Vec2 tri_coord = get_coordinates(local_intersection, a, b);
  const float coord_sum = tri_coord.x + tri_coord.y;

  if (tri_coord.x < 0.0 || tri_coord.y < 0.0 || coord_sum > 1.0) return std::optional<RayTriangleIntersection>();

  RayTriangleIntersection intersection;

  intersection.position = local_intersection + p_triangle.points[0];
  intersection.barycentric = Vec3(
    1.0f - coord_sum,
    tri_coord.x,
    tri_coord.y
  );

  // const Vec3 point =  * a + tri_coord.y * b;
  // const float error = length(local_intersection - point);
  // if (error > 0.01) std::cout << point << "  ;  " << local_intersection << std::endl;

  intersection.distance = distance(local_intersection, ray.origin);

  return intersection;
}
