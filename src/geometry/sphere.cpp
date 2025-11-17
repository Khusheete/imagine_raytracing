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

#include "thirdparty/kmath/angles.hpp"


void Sphere::build_arrays() {
  constexpr unsigned int polar_count = 20 , azimuth_count = 20;

  positions_array.resize(3 * polar_count * azimuth_count);
  normals_array.resize(3 * polar_count * azimuth_count);
  uvs_array.resize(2 * polar_count * azimuth_count);

  for (unsigned int polar_iterator = 0; polar_iterator < polar_count; ++polar_iterator) {
    float u = 1.0f - (float)(polar_iterator) / (float)(polar_count-1);
    float polar = u * M_PI;
    for (unsigned int azimuth_iterator = 0; azimuth_iterator < azimuth_count; ++azimuth_iterator) {
      unsigned int vertexIndex = polar_iterator + azimuth_iterator * polar_count;
      float v = (float)(azimuth_iterator) / (float)(azimuth_count-1);
      float azimuth = - M_PI + 2.0 * v * M_PI;

      kmath::Vec3 xyz = kmath::spherical_to_cartesian(1.0f, polar, azimuth);

      positions_array[3 * vertexIndex + 0] = center.x + radius * xyz.x;
      positions_array[3 * vertexIndex + 1] = center.y + radius * xyz.y;
      positions_array[3 * vertexIndex + 2] = center.z + radius * xyz.z;
      normals_array[3 * vertexIndex + 0] = xyz.x;
      normals_array[3 * vertexIndex + 1] = xyz.y;
      normals_array[3 * vertexIndex + 2] = xyz.z;
      uvs_array[2 * vertexIndex + 0] = u;
      uvs_array[2 * vertexIndex + 1] = v;
    }
  }

  triangles_array.clear();
  for (unsigned int polar_iterator = 0; polar_iterator < polar_count - 1; ++polar_iterator) {
    for (unsigned int azimuth_iterator = 0; azimuth_iterator < azimuth_count - 1; ++azimuth_iterator) {
      unsigned int vertexuv = polar_iterator + azimuth_iterator * polar_count;
      unsigned int vertexUv = polar_iterator + 1 + azimuth_iterator * polar_count;
      unsigned int vertexuV = polar_iterator + (azimuth_iterator+1) * polar_count;
      unsigned int vertexUV = polar_iterator + 1 + (azimuth_iterator+1) * polar_count;
      triangles_array.push_back(vertexuv);
      triangles_array.push_back(vertexUv);
      triangles_array.push_back(vertexUV);
      triangles_array.push_back(vertexuv);
      triangles_array.push_back(vertexUV);
      triangles_array.push_back(vertexuV);
    }
  }
}


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
  
  intersection.exists = true;
  return intersection;
}


void Sphere::translate(const kmath::Vec3 &p_translation) {
  center += p_translation;
  Mesh::translate(p_translation);
}


void Sphere::apply_transformation_matrix(const kmath::Mat3 &p_transform) {
  center = p_transform * center;
  Mesh::apply_transformation_matrix(p_transform); // FIXME: Does not work for non uniform scaling
}


Sphere::Sphere(): Mesh() {}

 
Sphere::Sphere(const kmath::Vec3 &p_center, const float p_radius) : Mesh(), center(p_center), radius(p_radius) {}
