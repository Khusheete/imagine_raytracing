#ifndef RAY_H
#define RAY_H


#include <cmath>
#include <ostream>

#include "kmath/vector.hpp"


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
