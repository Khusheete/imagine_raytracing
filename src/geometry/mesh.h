#ifndef MESH_H
#define MESH_H


#include <vector>
#include <string>
#include <cfloat>
#include <cstdint>

#include <GL/gl.h>

#include "kmath/matrix.hpp"
#include "kmath/vector.hpp"

#include "ray.h"
#include "triangle.h"
#include "Material.h"



// -------------------------------------------
// Basic Mesh class
// -------------------------------------------


struct MeshVertex {
    kmath::Vec3 position;
    kmath::Vec3 normal;
    kmath::Vec2 uv;

// public:
    // inline MeshVertex(const kmath::Vec3 &p_position, const kmath::Vec3 &p_normal, const kmath::Vec2 &p_uv = kmath::Vec2::ZERO)
    //     : position(p_position), normal(p_normal) , uv(p_uv) {}
};


struct MeshTriangle {
    uint32_t a = 0, b = 0, c = 0;

public:
    unsigned int &operator[](unsigned int iv) { return *(&a + iv); }
    unsigned int operator[](unsigned int iv) const { return *(&a + iv); }
};




class Mesh {
protected:
    void build_positions_array() {
        positions_array.resize(3 * vertices.size());
        for( unsigned int v = 0 ; v < vertices.size() ; ++v ) {
            positions_array[3*v + 0] = vertices[v].position.x;
            positions_array[3*v + 1] = vertices[v].position.y;
            positions_array[3*v + 2] = vertices[v].position.z;
        }
    }
    void build_normals_array() {
        normalsArray.resize( 3 * vertices.size() );
        for( unsigned int v = 0 ; v < vertices.size() ; ++v ) {
            normalsArray[3*v + 0] = vertices[v].normal.x;
            normalsArray[3*v + 1] = vertices[v].normal.y;
            normalsArray[3*v + 2] = vertices[v].normal.z;
        }
    }
    void build_UVs_array() {
        uvs_array.resize( 2 * vertices.size() );
        for( unsigned int vert = 0 ; vert < vertices.size() ; ++vert ) {
            uvs_array[2*vert + 0] = vertices[vert].uv.x;
            uvs_array[2*vert + 1] = vertices[vert].uv.y;
        }
    }
    void build_triangles_array() {
        triangles_array.resize( 3 * triangles.size() );
        for( unsigned int t = 0 ; t < triangles.size() ; ++t ) {
            triangles_array[3*t + 0] = triangles[t][0];
            triangles_array[3*t + 1] = triangles[t][1];
            triangles_array[3*t + 2] = triangles[t][2];
        }
    }


public:
    std::vector<MeshVertex> vertices;
    std::vector<MeshTriangle> triangles;

    std::vector<float> positions_array;
    std::vector<float> normalsArray;
    std::vector<float> uvs_array;
    std::vector<unsigned int> triangles_array;

    Material material;

    void loadOFF (const std::string & filename);
    void recomputeNormals ();
    void centerAndScaleToUnit ();
    void scaleUnit ();


    virtual void build_arrays() {
        recomputeNormals();
        build_positions_array();
        build_normals_array();
        build_UVs_array();
        build_triangles_array();
    }


    void translate(const kmath::Vec3 & p_translation){
        for( unsigned int v = 0 ; v < vertices.size() ; ++v ) {
            vertices[v].position += p_translation;
        }
    }


    void apply_transformation_matrix(const kmath::Mat3 &p_transform){
        for( unsigned int v = 0 ; v < vertices.size() ; ++v ) {
            vertices[v].position = p_transform * vertices[v].position;
        }

        //        recomputeNormals();
        //        build_positions_array();
        //        build_normals_array();
    }


    void scale(const kmath::Vec3 &p_scale){
        kmath::Mat3 scale_matrix(
            kmath::Vec3(p_scale.x, 0., 0.),
            kmath::Vec3(0., p_scale.y, 0.),
            kmath::Vec3(0., 0., p_scale.z)
        ); // Matrice de transformation de mise à l'échelle
        apply_transformation_matrix(scale_matrix);
    }


    void rotate_x(const float p_angle_degrees) {
        float angle = p_angle_degrees * M_PI / 180.;
        kmath::Mat3 rotation = kmath::Mat3::x_rotation(angle);
        apply_transformation_matrix(rotation);
    }


    void rotate_y(const float p_angle_degrees) {
        float angle = p_angle_degrees * M_PI / 180.;
        kmath::Mat3 rotation = kmath::Mat3::y_rotation(angle);
        apply_transformation_matrix(rotation);
    }


    void rotate_z(const float p_angle_degrees) {
        float angle = p_angle_degrees * M_PI / 180.;
        kmath::Mat3 rotation = kmath::Mat3::y_rotation(angle);
        apply_transformation_matrix(rotation);
    }


    void draw() const {
        if( triangles_array.size() == 0 ) return;
        GLfloat material_color[4] = {material.diffuse_material.x,
                                     material.diffuse_material.y,
                                     material.diffuse_material.z,
                                     1.0};

        GLfloat material_specular[4] = {material.specular_material.x,
                                        material.specular_material.y,
                                        material.specular_material.z,
                                        1.0};

        GLfloat material_ambient[4] = {material.ambient_material.x,
                                       material.ambient_material.y,
                                       material.ambient_material.z,
                                       1.0};

        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_color);
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.shininess);

        glEnableClientState(GL_VERTEX_ARRAY) ;
        glEnableClientState (GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 3*sizeof (float), (GLvoid*)(normalsArray.data()));
        glVertexPointer(3, GL_FLOAT, 3*sizeof (float) , (GLvoid*)(positions_array.data()));
        glDrawElements(GL_TRIANGLES, triangles_array.size(), GL_UNSIGNED_INT, (GLvoid*)(triangles_array.data()));

    }

    RayTriangleIntersection intersect(const Ray &p_ray) const {
        RayTriangleIntersection closestIntersection;
        closestIntersection.t = FLT_MAX;
        // Note :
        // Creer un objet Triangle pour chaque face
        // Vous constaterez des problemes de précision
        // solution : ajouter un facteur d'échelle lors de la création du Triangle : float triangleScaling = 1.000001;
        return closestIntersection;
    }


    virtual ~Mesh() = default;
};




#endif
