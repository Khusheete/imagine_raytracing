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


#include "sphere.hpp"


RaySphereIntersection Sphere::intersect(const Ray &p_ray) const {
  RaySphereIntersection intersection;
  const kmath::Vec3 alpha = center - p_ray.origin;
  const float a = kmath::length_squared(p_ray.direction);
  const float b = 2.0f * kmath::dot(alpha, p_ray.direction);
  const float c = kmath::length_squared(alpha) - radius * radius;
  const float delta = b * b - 4.0f * a * c;

  if (delta < 0) { // The sphere is not hit
    return intersection;
  }

  const float sqrt_delta = std::sqrt(delta);
  const float x1a = b - sqrt_delta;

  float dist = 0.0f;

  if (x1a < 0.0) { // The collision is behind the ray, either we're in the sphere, or the sphere's behind us
    const float x2a = b + sqrt_delta;
    if (x2a < 0.0) return intersection; // The sphere is behind the ray, no hit
    dist = 0.5f * x2a / a;
  } else {
    dist = 0.5f * x1a / a;
  }

  kmath::Vec3 pos = p_ray.origin + dist * p_ray.direction;
  intersection.distance = dist;
  intersection.position = pos;
  intersection.normal = kmath::normalized(pos - center);
  // TODO: calculate uv component
  
  intersection.exists = true;
  return intersection;
}


