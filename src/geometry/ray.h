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
    float theta, phi;
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

// Returns the orthogonal distance between p_point and p_line
float distance_squared(const kmath::Vec3 &p_point, const Ray &p_ray); // TODO: implement


inline float distance(const kmath::Vec3 &p_point, const Ray &p_ray) {
    return std::sqrt(distance_squared(p_point, p_ray));
}


std::ostream &operator<<(std::ostream &p_stream, const Ray &p_ray);


#endif // #ifndef RAY_H
