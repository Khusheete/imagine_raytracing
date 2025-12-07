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


#pragma once


#include "geometry/ray.hpp"
#include "thirdparty/kmath/vector.hpp"
#include <optional>


struct RayTriangleIntersection {
  kmath::Vec3 position;
  kmath::Vec3 barycentric;
  float distance;
};


struct Triangle {
  kmath::Vec3 points[3];
};


// Returns (lambda_1, lambda_2) such that p_point = lambda_1 * p_a + lambda_2 * p_b
kmath::Vec2 get_coordinates(const kmath::Vec3 &p_point, const kmath::Vec3 &p_a, const kmath::Vec3 &p_b);

std::optional<RayTriangleIntersection> get_intersection(const Ray &p_ray, const Triangle &p_triangle);


// class Triangle {
// private:
//     Vec3 m_c[3] , m_normal;
//     float area;
// public:
//     Triangle() {}
//     Triangle( Vec3 const & c0 , Vec3 const & c1 , Vec3 const & c2 ) {
//         m_c[0] = c0;
//         m_c[1] = c1;
//         m_c[2] = c2;
//         updateAreaAndNormal();
//     }
//     void updateAreaAndNormal() {
//         Vec3 nNotNormalized = Vec3::cross( m_c[1] - m_c[0] , m_c[2] - m_c[0] );
//         float norm = nNotNormalized.length();
//         m_normal = nNotNormalized / norm;
//         area = norm / 2.f;
//     }
//     void setC0( Vec3 const & c0 ) { m_c[0] = c0; } // remember to update the area and normal afterwards!
//     void setC1( Vec3 const & c1 ) { m_c[1] = c1; } // remember to update the area and normal afterwards!
//     void setC2( Vec3 const & c2 ) { m_c[2] = c2; } // remember to update the area and normal afterwards!
//     Vec3 const & normal() const { return m_normal; }
//     Vec3 projectOnSupportPlane([[maybe_unused]] Vec3 const & p ) const {
//         Vec3 result;
//         //TODO completer
//         return result;
//     }
//     float squareDistanceToSupportPlane([[maybe_unused]] Vec3 const & p ) const {
//         float result = 0.0f;
//         //TODO completer
//         return result;
//     }
//     float distanceToSupportPlane( Vec3 const & p ) const { return sqrt( squareDistanceToSupportPlane(p) ); }
//     bool isParallelTo([[maybe_unused]] Line const & L ) const {
//         bool result = false;
//         //TODO completer
//         return result;
//     }
//     Vec3 getIntersectionPointWithSupportPlane([[maybe_unused]] Line const & L ) const {
//         // you should check first that the line is not parallel to the plane!
//         Vec3 result;
//         //TODO completer
//         return result;
//     }
//     void computeBarycentricCoordinates([[maybe_unused]] Vec3 const & p ,[[maybe_unused]] float & u0 ,[[maybe_unused]] float & u1 ,[[maybe_unused]] float & u2 ) const {
//         //TODO Complete
//     }

//     RayTriangleIntersection getIntersection([[maybe_unused]] Ray const & ray ) const {
//         RayTriangleIntersection result;
//         // 1) check that the ray is not parallel to the triangle:

//         // 2) check that the triangle is "in front of" the ray:

//         // 3) check that the intersection point is inside the triangle:
//         // CONVENTION: compute u,v such that p = w0*c0 + w1*c1 + w2*c2, check that 0 <= w0,w1,w2 <= 1

//         // 4) Finally, if all conditions were met, then there is an intersection! :

//         return result;
//     }
// };

