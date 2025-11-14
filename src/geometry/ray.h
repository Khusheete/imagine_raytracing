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


#ifndef RAY_H
#define RAY_H


#include <cmath>
#include <ostream>

#include "kmath/vector.hpp"


struct RaySphereIntersection {
  kmath::Vec3 position;
  kmath::Vec3 normal;
  float distance;
  bool exists = false;
  float polar, azimuth;
};


struct RayTriangleIntersection {
  kmath::Vec3 position;
  kmath::Vec3 normal;
  float distance;
  bool exists = false;
  kmath::Vec3 barycentric[3];
};


struct RaySquareIntersection {
  kmath::Vec3 position;
  kmath::Vec3 normal;
  float distance;
  bool exists = false;
  kmath::Vec2 uv;
};


struct RayIntersection {
public:
  union PolyIntersection {
    RaySphereIntersection rsph;
    RayTriangleIntersection rtri;
    RaySquareIntersection rsqu;

    struct Common {
      kmath::Vec3 position;
      kmath::Vec3 normal;
      float distance;
      bool exists = false;
    } common;


    PolyIntersection(RaySphereIntersection p_rsph);
    PolyIntersection(RayTriangleIntersection p_rsph);
    PolyIntersection(RaySquareIntersection p_rsph);
    PolyIntersection();
  } intersection;

  size_t element_id;

  enum class Kind : unsigned char {
    NONE,
    RAY_SPHERE,
    RAY_TRIANGLE,
    RAY_SQUARE,
  } kind = Kind::NONE;


public:
  static RayIntersection from(RaySphereIntersection p_rsph);
  static RayIntersection from(RayTriangleIntersection p_rtri);
  static RayIntersection from(RaySquareIntersection p_rsqu);
};


struct Ray {
  kmath::Vec3 origin, direction;

public:
  Ray() = default;
  Ray(const kmath::Vec3 &p_origin, const kmath::Vec3 &p_direction);

  kmath::Vec3 get_moment() const;
};


// Project p_point onto p_line
kmath::Vec3 project(const kmath::Vec3 &p_point, const Ray &p_ray); // TODO: implement
// Reflect p_direction given the normal p_normal
kmath::Vec3 reflect(const kmath::Vec3 &p_direction, const kmath::Vec3 &p_normal);

// Returns the orthogonal distance between p_point and p_line
float distance_squared(const kmath::Vec3 &p_point, const Ray &p_ray); // TODO: implement


inline float distance(const kmath::Vec3 &p_point, const Ray &p_ray) {
  return std::sqrt(distance_squared(p_point, p_ray));
}


std::ostream &operator<<(std::ostream &p_stream, const Ray &p_ray);


#endif // #ifndef RAY_H
