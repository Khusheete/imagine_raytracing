#include "ray.h"

#include "kmath/print.hpp"
#include "kmath/vector.hpp"
#include <limits>


RayIntersection RayIntersection::from(const RaySphereIntersection p_rsph) {
  return RayIntersection(
    PolyIntersection(p_rsph), -1, Kind::RAY_SPHERE
  );
}
RayIntersection RayIntersection::from(const RayTriangleIntersection p_rtri) {
  return RayIntersection(
    PolyIntersection(p_rtri), -1, Kind::RAY_TRIANGLE
  );
}
RayIntersection RayIntersection::from(const RaySquareIntersection p_rsqu) {
  return RayIntersection(
    PolyIntersection(p_rsqu), -1, Kind::RAY_SQUARE
  );
}


RayIntersection::PolyIntersection::PolyIntersection(RaySphereIntersection p_rsph): rsph(p_rsph) {}
RayIntersection::PolyIntersection::PolyIntersection(RayTriangleIntersection p_rtri): rtri(p_rtri) {}
RayIntersection::PolyIntersection::PolyIntersection(RaySquareIntersection p_rsqu): rsqu(p_rsqu) {}
RayIntersection::PolyIntersection::PolyIntersection(): common(kmath::Vec3::ZERO, kmath::Vec3::ZERO, std::numeric_limits<float>::infinity(), false) {}


std::ostream &operator<<(std::ostream &p_stream, const Ray &p_ray) {
  p_stream << "Line(" << p_ray.origin << ", " << p_ray.direction << ")";
  return p_stream;
}


Ray::Ray(const kmath::Vec3 &p_origin, const kmath::Vec3 &p_direction): origin(p_origin), direction(kmath::normalized(p_direction)) {}
