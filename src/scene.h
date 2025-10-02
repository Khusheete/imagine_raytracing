#ifndef SCENE_H
#define SCENE_H

#include <limits>
#include <vector>

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

struct RaySceneIntersection{
    bool intersectionExists;
    unsigned int typeOfIntersectedObject;
    unsigned int objectIndex;
    float t;
    RayTriangleIntersection rayMeshIntersection;
    RaySphereIntersection raySphereIntersection;
    RaySquareIntersection raySquareIntersection;
    RaySceneIntersection() : intersectionExists(false) , t(FLT_MAX) {}
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


    RaySceneIntersection computeIntersection([[maybe_unused]] Ray const & ray) {
        RaySceneIntersection result;
        //TODO calculer les intersections avec les objets de la scene et garder la plus proche
        return result;
    }


    kmath::Vec3 rayTraceRecursive([[maybe_unused]] Ray ray ,[[maybe_unused]] int NRemainingBounces ) {

        //TODO RaySceneIntersection raySceneIntersection = computeIntersection(ray);
        kmath::Vec3 color;
        return color;
    }


    kmath::Vec3 rayTrace([[maybe_unused]] Ray const & rayStart ) {
        float min_distance = std::numeric_limits<float>::infinity();
        Sphere *closest = nullptr;
        for (Sphere &sphere : spheres) {
            RaySphereIntersection inter = sphere.intersect(rayStart);
            if (inter.exists && inter.distance < min_distance) {
                min_distance = inter.distance;
                closest = &sphere;
            }
        }

        if (closest != nullptr) {
            return closest->material.diffuse_material;
        }
        return kmath::Vec3(0.0, 0.0, 0.0);
    }


    void setup_single_sphere() {
        meshes.clear();
        spheres.clear();
        squares.clear();
        lights.clear();

        {
            lights.resize( lights.size() + 1 );
            Light & light = lights[lights.size() - 1];
            light.pos = kmath::Vec3(-5,5,5);
            light.radius = 2.5f;
            light.powerCorrection = 2.f;
            light.type = LightType_Spherical;
            light.material = kmath::Vec3(1,1,1);
            light.isInCamSpace = false;
        }
        {
            spheres.resize( spheres.size() + 1 );
            Sphere & s = spheres[spheres.size() - 1];
            s.center = kmath::Vec3(0. , 0. , 0.);
            s.radius = 1.f;
            s.build_arrays();
            s.material.type = Material_Mirror;
            s.material.diffuse_material = kmath::Vec3( 1.,1.,1 );
            s.material.specular_material = kmath::Vec3( 0.2,0.2,0.2 );
            s.material.shininess = 20;
        }
    }

    void setup_single_square() {
        meshes.clear();
        spheres.clear();
        squares.clear();
        lights.clear();

        {
            lights.resize( lights.size() + 1 );
            Light & light = lights[lights.size() - 1];
            light.pos = kmath::Vec3(-5,5,5);
            light.radius = 2.5f;
            light.powerCorrection = 2.f;
            light.type = LightType_Spherical;
            light.material = kmath::Vec3(1,1,1);
            light.isInCamSpace = false;
        }

        {
            squares.resize( squares.size() + 1 );
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3( 0.8,0.8,0.8 );
            s.material.specular_material = kmath::Vec3( 0.8,0.8,0.8 );
            s.material.shininess = 20;
        }
    }

    void setup_cornell_box(){
        meshes.clear();
        spheres.clear();
        squares.clear();
        lights.clear();

        {
            lights.resize( lights.size() + 1 );
            Light & light = lights[lights.size() - 1];
            light.pos = kmath::Vec3( 0.0, 1.5, 0.0 );
            light.radius = 2.5f;
            light.powerCorrection = 2.f;
            light.type = LightType_Spherical;
            light.material = kmath::Vec3(1,1,1);
            light.isInCamSpace = false;
        }

        { //Back Wall
            squares.resize( squares.size() + 1 );
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.scale(kmath::Vec3(2., 2., 1.));
            s.translate(kmath::Vec3(0., 0., -2.));
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3( 1.,1.,1. );
            s.material.specular_material = kmath::Vec3( 1.,1.,1. );
            s.material.shininess = 16;
        }

        { //Left Wall

            squares.resize( squares.size() + 1 );
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.scale(kmath::Vec3(2., 2., 1.));
            s.translate(kmath::Vec3(0., 0., -2.));
            s.rotate_y(90);
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3( 1.,0.,0. );
            s.material.specular_material = kmath::Vec3( 1.,0.,0. );
            s.material.shininess = 16;
        }

        { //Right Wall
            squares.resize( squares.size() + 1 );
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.translate(kmath::Vec3(0., 0., -2.));
            s.scale(kmath::Vec3(2., 2., 1.));
            s.rotate_y(-90);
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3( 0.0,1.0,0.0 );
            s.material.specular_material = kmath::Vec3( 0.0,1.0,0.0 );
            s.material.shininess = 16;
        }

        { //Floor
            squares.resize( squares.size() + 1 );
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.translate(kmath::Vec3(0., 0., -2.));
            s.scale(kmath::Vec3(2., 2., 1.));
            s.rotate_x(-90);
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3( 1.0,1.0,1.0 );
            s.material.specular_material = kmath::Vec3( 1.0,1.0,1.0 );
            s.material.shininess = 16;
        }

        { //Ceiling
            squares.resize( squares.size() + 1 );
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.translate(kmath::Vec3(0., 0., -2.));
            s.scale(kmath::Vec3(2., 2., 1.));
            s.rotate_x(90);
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3( 1.0,1.0,1.0 );
            s.material.specular_material = kmath::Vec3( 1.0,1.0,1.0 );
            s.material.shininess = 16;
        }

        { //Front Wall
            squares.resize( squares.size() + 1 );
            Square & s = squares[squares.size() - 1];
            s.setQuad(kmath::Vec3(-1., -1., 0.), kmath::Vec3(1., 0, 0.), kmath::Vec3(0., 1, 0.), 2., 2.);
            s.translate(kmath::Vec3(0., 0., -2.));
            s.scale(kmath::Vec3(2., 2., 1.));
            s.rotate_y(180);
            s.build_arrays();
            s.material.diffuse_material = kmath::Vec3( 1.0,1.0,1.0 );
            s.material.specular_material = kmath::Vec3( 1.0,1.0,1.0 );
            s.material.shininess = 16;
        }


        { //GLASS Sphere

            spheres.resize( spheres.size() + 1 );
            Sphere & s = spheres[spheres.size() - 1];
            s.center = kmath::Vec3(1.0, -1.25, 0.5);
            s.radius = 0.75f;
            s.build_arrays();
            s.material.type = Material_Mirror;
            s.material.diffuse_material = kmath::Vec3( 1.,0.,0. );
            s.material.specular_material = kmath::Vec3( 1.,0.,0. );
            s.material.shininess = 16;
            s.material.transparency = 1.0;
            s.material.index_medium = 1.4;
        }


        { //MIRRORED Sphere
            spheres.resize( spheres.size() + 1 );
            Sphere & s = spheres[spheres.size() - 1];
            s.center = kmath::Vec3(-1.0, -1.25, -0.5);
            s.radius = 0.75f;
            s.build_arrays();
            s.material.type = Material_Glass;
            s.material.diffuse_material = kmath::Vec3( 1.,1.,1. );
            s.material.specular_material = kmath::Vec3(  1.,1.,1. );
            s.material.shininess = 16;
            s.material.transparency = 0.;
            s.material.index_medium = 0.;
        }

    }

};



#endif // #ifndef SCENE_H
