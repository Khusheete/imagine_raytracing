#include "ray.h"

#include "kmath/print.hpp"


std::ostream &operator<<(std::ostream &p_stream, const Ray &p_ray) {
    p_stream << "Line(" << p_ray.origin << ", " << p_ray.direction << ")";
    return p_stream;
}


Ray::Ray(const kmath::Vec3 &p_origin, const kmath::Vec3 &p_direction): origin(p_origin), direction(kmath::normalized(p_direction)) {}
