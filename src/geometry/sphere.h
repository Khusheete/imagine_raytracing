#ifndef Sphere_H
#define Sphere_H

#include "kmath/vector.hpp"
#include "mesh.h"
#include <cmath>


struct RaySphereIntersection {
    bool exists = false;
    kmath::Vec3 position;
    kmath::Vec3 normal;
    float distance;
    float theta, phi;
};


static kmath::Vec3 SphericalCoordinatesToEuclidean( kmath::Vec3 ThetaPhiR ) {
    return ThetaPhiR.z * kmath::Vec3(cos(ThetaPhiR.x) * cos(ThetaPhiR.y), sin(ThetaPhiR.x) * cos(ThetaPhiR.y), sin(ThetaPhiR.y));
}

static kmath::Vec3 SphericalCoordinatesToEuclidean( float theta , float phi ) {
    return kmath::Vec3( cos(theta) * cos(phi) , sin(theta) * cos(phi) , sin(phi) );
}

static kmath::Vec3 EuclideanCoordinatesToSpherical(kmath::Vec3 xyz) {
    float R = kmath::length(xyz);
    float phi = asin(xyz.z / R);
    float theta = atan2(xyz.y, xyz.x);
    return kmath::Vec3(theta, phi, R);
}



class Sphere : public Mesh {
public:
    kmath::Vec3 center;
    float radius;

    Sphere() : Mesh() {}
    Sphere(kmath::Vec3 c , float r) : Mesh() , center(c) , radius(r) {}
    virtual ~Sphere() = default;


    void build_arrays() {
        unsigned int nTheta = 20 , nPhi = 20;
        positions_array.resize(3 * nTheta * nPhi );
        normalsArray.resize(3 * nTheta * nPhi );
        uvs_array.resize(2 * nTheta * nPhi );
        for( unsigned int thetaIt = 0 ; thetaIt < nTheta ; ++thetaIt ) {
            float u = (float)(thetaIt) / (float)(nTheta-1);
            float theta = u * 2 * M_PI;
            for( unsigned int phiIt = 0 ; phiIt < nPhi ; ++phiIt ) {
                unsigned int vertexIndex = thetaIt + phiIt * nTheta;
                float v = (float)(phiIt) / (float)(nPhi-1);
                float phi = - M_PI/2.0 + v * M_PI;
                kmath::Vec3 xyz = SphericalCoordinatesToEuclidean( theta , phi );
                positions_array[3 * vertexIndex + 0] = center.x + radius * xyz.x;
                positions_array[3 * vertexIndex + 1] = center.y + radius * xyz.y;
                positions_array[3 * vertexIndex + 2] = center.z + radius * xyz.z;
                normalsArray[3 * vertexIndex + 0] = xyz.x;
                normalsArray[3 * vertexIndex + 1] = xyz.y;
                normalsArray[3 * vertexIndex + 2] = xyz.z;
                uvs_array[2 * vertexIndex + 0] = u;
                uvs_array[2 * vertexIndex + 1] = v;
            }
        }
        triangles_array.clear();
        for( unsigned int thetaIt = 0 ; thetaIt < nTheta - 1 ; ++thetaIt ) {
            for( unsigned int phiIt = 0 ; phiIt < nPhi - 1 ; ++phiIt ) {
                unsigned int vertexuv = thetaIt + phiIt * nTheta;
                unsigned int vertexUv = thetaIt + 1 + phiIt * nTheta;
                unsigned int vertexuV = thetaIt + (phiIt+1) * nTheta;
                unsigned int vertexUV = thetaIt + 1 + (phiIt+1) * nTheta;
                triangles_array.push_back(vertexuv);
                triangles_array.push_back(vertexUv);
                triangles_array.push_back(vertexUV);
                triangles_array.push_back(vertexuv);
                triangles_array.push_back(vertexUV);
                triangles_array.push_back(vertexuV);
            }
        }
    }


    RaySphereIntersection intersect(const Ray &p_ray) const {
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
};
#endif
