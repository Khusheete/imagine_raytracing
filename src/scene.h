#ifndef SCENE_H
#define SCENE_H

#include <limits>
#include <vector>

#include "geometry/ray.h"
#include "kmath/vector.hpp"

#include "geometry/mesh.h"
#include "geometry/sphere.h"
#include "geometry/square.h"

#include "utils/gl_utils.hpp"


enum LightType {
    LightType_Spherical,
    LightType_Quad
};


struct Light {
    kmath::Vec3 material;
    bool isInCamSpace;
    LightType type;

    kmath::Vec3 pos;
    float radius;

    Mesh quad;

    float powerCorrection;

    Light() : powerCorrection(1.0) {}

};


class Scene {
    std::vector<Mesh> meshes;
    std::vector<Sphere> spheres;
    std::vector<Square> squares;
    std::vector<Light> lights;

public:


    Scene() = default;
    ~Scene() = default;

    void draw() {
        // iterer sur l'ensemble des objets, et faire leur rendu :
        for(unsigned int It = 0 ; It < meshes.size() ; ++It) {
            Mesh const & mesh = meshes[It];
            mesh.draw();
        }
        for(unsigned int It = 0 ; It < spheres.size() ; ++It) {
            Sphere const & sphere = spheres[It];
            sphere.draw();
        }
        for(unsigned int It = 0 ; It < squares.size() ; ++It) {
            Square const & square = squares[It];
            square.draw();
        }
    }


    RayIntersection compute_intersection(const Ray &p_ray) {
        RayIntersection result;
        
        // Sphere intersection
        for (size_t i = 0; i < spheres.size(); i++) {
            RaySphereIntersection rsph = spheres[i].intersect(p_ray);
            if (rsph.exists && rsph.distance < result.intersection.common.distance) {
                result.intersection.rsph = rsph;
                result.element_id = i;
                result.kind = RayIntersection::Kind::RAY_SPHERE;
            }
        }
        return result;
    }


    kmath::Vec3 ray_trace_recursive([[maybe_unused]] Ray ray, [[maybe_unused]] int NRemainingBounces) {

        //TODO RaySceneIntersection raySceneIntersection = computeIntersection(ray);
        kmath::Vec3 color;
        return color;
    }


    kmath::Vec3 ray_trace(const Ray &p_ray_start) {
        RayIntersection inter = compute_intersection(p_ray_start);

        switch (inter.kind) {
        case RayIntersection::Kind::RAY_SPHERE: {
            const Sphere &sphere = spheres[inter.element_id];
            return sphere.material.diffuse_material;
        }
        case RayIntersection::Kind::RAY_TRIANGLE:
        case RayIntersection::Kind::RAY_SQUARE:
        case RayIntersection::Kind::NONE:
            return kmath::Vec3::ZERO;
        }
    }


    void setup_single_sphere() {
        meshes.clear();
        spheres.clear();
        squares.clear();
        lights.clear();

        {
            lights.resize(lights.size() + 1);
            Light & light = lights[lights.size() - 1];
            light.pos = kmath::Vec3(-5,5,5);
            light.radius = 2.5f;
            light.powerCorrection = 2.f;
            light.type = LightType_Spherical;
            light.material = kmath::Vec3(1,1,1);
            light.isInCamSpace = false;
        }
        {
            spheres.resize(spheres.size() + 1);
            Sphere & s = spheres[spheres.size() - 1];
            s.center = kmath::Vec3(0. , 0. , 0.);
            s.radius = 1.f;
            s.build_arrays();
            s.material.type = Material_Mirror;
            s.material.diffuse_material = kmath::Vec3(1., 1., 1);
            s.material.specular_material = kmath::Vec3(0.2, 0.2, 0.2);
            s.material.shininess = 20;
        }
    }

    void setup_single_square() {
        meshes.clear();
        spheres.clear();
        squares.clear();
        lights.clear();

        {
            lights.resize(lights.size() + 1);
            Light & light = lights[lights.size() - 1];
            light.pos = kmath::Vec3(-5, 5, 5);
            light.radius = 2.5f;
            light.powerCorrection = 2.f;
            light.type = LightType_Spherical;
            light.material = kmath::Vec3(1, 1, 1);
            light.isInCamSpace = false;
        }

        {
            squares.resize(squares.size() + 1);
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3(0.8, 0.8, 0.8);
            s.material.specular_material = kmath::Vec3(0.8, 0.8, 0.8);
            s.material.shininess = 20;
        }
    }

    void setup_cornell_box(){
        meshes.clear();
        spheres.clear();
        squares.clear();
        lights.clear();

        {
            lights.resize(lights.size() + 1);
            Light &light = lights[lights.size() - 1];
            light.pos = kmath::Vec3(0.0, 1.5, 0.0);
            light.radius = 2.5f;
            light.powerCorrection = 2.f;
            light.type = LightType_Spherical;
            light.material = kmath::Vec3(1, 1, 1);
            light.isInCamSpace = false;
        }

        { //Back Wall
            squares.resize(squares.size() + 1);
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.scale(kmath::Vec3(2., 2., 1.));
            s.translate(kmath::Vec3(0., 0., -2.));
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3(1., 1., 1.);
            s.material.specular_material = kmath::Vec3(1., 1., 1.);
            s.material.shininess = 16;
        }

        { //Left Wall

            squares.resize(squares.size() + 1);
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.scale(kmath::Vec3(2., 2., 1.));
            s.translate(kmath::Vec3(0., 0., -2.));
            s.rotate_y(90);
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3(1., 0., 0.);
            s.material.specular_material = kmath::Vec3(1., 0., 0.);
            s.material.shininess = 16;
        }

        { //Right Wall
            squares.resize(squares.size() + 1);
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.translate(kmath::Vec3(0., 0., -2.));
            s.scale(kmath::Vec3(2., 2., 1.));
            s.rotate_y(-90);
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3(0.0, 1.0, 0.0);
            s.material.specular_material = kmath::Vec3(0.0, 1.0, 0.0);
            s.material.shininess = 16;
        }

        { //Floor
            squares.resize(squares.size() + 1);
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.translate(kmath::Vec3(0., 0., -2.));
            s.scale(kmath::Vec3(2., 2., 1.));
            s.rotate_x(-90);
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3(1.0, 1.0, 1.0);
            s.material.specular_material = kmath::Vec3(1.0, 1.0, 1.0);
            s.material.shininess = 16;
        }

        { //Ceiling
            squares.resize(squares.size() + 1);
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.translate(kmath::Vec3(0., 0., -2.));
            s.scale(kmath::Vec3(2., 2., 1.));
            s.rotate_x(90);
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3(1.0, 1.0, 1.0);
            s.material.specular_material = kmath::Vec3(1.0, 1.0, 1.0);
            s.material.shininess = 16;
        }

        { //Front Wall
            squares.resize(squares.size() + 1);
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.translate(kmath::Vec3(0., 0., -2.));
            s.scale(kmath::Vec3(2., 2., 1.));
            s.rotate_y(180);
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3(1.0, 1.0, 1.0);
            s.material.specular_material = kmath::Vec3(1.0, 1.0, 1.0);
            s.material.shininess = 16;
        }


        { //GLASS Sphere

            spheres.resize(spheres.size() + 1);
            Sphere & s = spheres[spheres.size() - 1];
            s.center = kmath::Vec3(1.0, -1.25, 0.5);
            s.radius = 0.75f;
            s.build_arrays();
            s.material.type = Material_Mirror;
            s.material.diffuse_material = kmath::Vec3(1., 0., 0.);
            s.material.specular_material = kmath::Vec3(1., 0., 0.);
            s.material.shininess = 16;
            s.material.transparency = 1.0;
            s.material.index_medium = 1.4;
        }


        { //MIRRORED Sphere
            spheres.resize(spheres.size() + 1);
            Sphere & s = spheres[spheres.size() - 1];
            s.center = kmath::Vec3(-1.0, -1.25, -0.5);
            s.radius = 0.75f;
            s.build_arrays();
            s.material.type = Material_Glass;
            s.material.diffuse_material = kmath::Vec3(1., 1., 1.);
            s.material.specular_material = kmath::Vec3(1., 1., 1.);
            s.material.shininess = 16;
            s.material.transparency = 0.;
            s.material.index_medium = 0.;
        }

    }

};



#endif // #ifndef SCENE_H
