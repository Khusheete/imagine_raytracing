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


#include "ray.hpp"

#include "thirdparty/kmath/print.hpp"
#include "thirdparty/kmath/vector.hpp"
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


kmath::Vec3 reflect(const kmath::Vec3 &p_direction, const kmath::Vec3 &p_normal) {
  const float dist = kmath::dot(p_direction, p_normal);
  const float normal_len2 = kmath::length_squared(p_normal);
  const kmath::Vec3 along_normal = p_normal * (2.0f * dist / normal_len2);
  return p_direction - along_normal;
}


std::ostream &operator<<(std::ostream &p_stream, const Ray &p_ray) {
  p_stream << "Line(" << p_ray.origin << ", " << p_ray.direction << ")";
  return p_stream;
}


Ray::Ray(const kmath::Vec3 &p_origin, const kmath::Vec3 &p_direction): origin(p_origin), direction(kmath::normalized(p_direction)) {}
