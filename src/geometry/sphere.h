#ifndef Sphere_H
#define Sphere_H

#include "kmath/vector.hpp"
#include "kmath/angles.hpp"
#include "mesh.h"
#include <cmath>



class Sphere : public Mesh {
public:
    kmath::Vec3 center;
    float radius;

    Sphere() : Mesh() {}
    Sphere(kmath::Vec3 c , float r) : Mesh() , center(c) , radius(r) {}
    virtual ~Sphere() = default;


    void build_arrays() {
        constexpr unsigned int polar_count = 20 , azimuth_count = 20;

        positions_array.resize(3 * polar_count * azimuth_count);
        normalsArray.resize(3 * polar_count * azimuth_count);
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
                normalsArray[3 * vertexIndex + 0] = xyz.x;
                normalsArray[3 * vertexIndex + 1] = xyz.y;
                normalsArray[3 * vertexIndex + 2] = xyz.z;
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
