#ifndef MATERIAL_H
#define MATERIAL_H

#include "kmath/vector.hpp"

#include <cmath>


enum MaterialType {
    Material_Diffuse_Blinn_Phong ,
    Material_Glass,
    Material_Mirror
};


struct Material {
    kmath::Vec3 ambient_material;
    kmath::Vec3 diffuse_material;
    kmath::Vec3 specular_material;
    double shininess;

    float index_medium;
    float transparency;

    MaterialType type;

    Material() {
        type = Material_Diffuse_Blinn_Phong;
        transparency = 0.0;
        index_medium = 1.0;
        ambient_material = kmath::Vec3(0., 0., 0.);
    }
};



#endif // MATERIAL_H
