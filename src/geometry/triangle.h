#ifndef TRIANGLE_H
#define TRIANGLE_H


#include "kmath/vector.hpp"
// #include "ray.h"
// #include "Plane.h"


struct Triangle {
  kmath::Vec3 points[3];
  kmath::Vec3 normal;
  float area;
};


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


#endif
