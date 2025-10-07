#ifndef SQUARE_H
#define SQUARE_H
#include <vector>

#include "kmath/vector.hpp"

#include "mesh.h"
#include <cmath>


class Square : public Mesh {
public:
    kmath::Vec3 m_normal;
    kmath::Vec3 m_bottom_left;
    kmath::Vec3 m_right_vector;
    kmath::Vec3 m_up_vector;

    Square() : Mesh() {}
    Square(kmath::Vec3 const & bottomLeft , kmath::Vec3 const & rightVector , kmath::Vec3 const & upVector , float width=1. , float height=1. ,
           float uMin = 0.f , float uMax = 1.f , float vMin = 0.f , float vMax = 1.f) : Mesh() {
        setQuad(bottomLeft, rightVector, upVector, width, height, uMin, uMax, vMin, vMax);
    }
    virtual ~Square() = default;

    void setQuad( kmath::Vec3 const & bottomLeft , kmath::Vec3 const & rightVector , kmath::Vec3 const & upVector , float width=1. , float height=1. ,
                  float uMin = 0.f , float uMax = 1.f , float vMin = 0.f , float vMax = 1.f) {
        m_right_vector = kmath::normalized(rightVector);
        m_up_vector = kmath::normalized(upVector);
        m_normal = kmath::normalized(kmath::cross(rightVector , upVector));
        m_bottom_left = bottomLeft;

        m_right_vector = m_right_vector*width;
        m_up_vector = m_up_vector*height;

        vertices.clear();
        vertices.resize(4);
        vertices[0].position = bottomLeft;                                      vertices[0].uv = kmath::Vec2(uMin, vMin);
        vertices[1].position = bottomLeft + m_right_vector;                     vertices[1].uv = kmath::Vec2(uMax, vMin);
        vertices[2].position = bottomLeft + m_right_vector + m_up_vector;       vertices[2].uv = kmath::Vec2(uMax, vMax);
        vertices[3].position = bottomLeft + m_up_vector;                        vertices[3].uv = kmath::Vec2(uMin, vMax);
        vertices[0].normal = vertices[1].normal = vertices[2].normal = vertices[3].normal = m_normal;
        triangles.clear();
        triangles.resize(2);
        triangles[0][0] = 0;
        triangles[0][1] = 1;
        triangles[0][2] = 2;
        triangles[1][0] = 0;
        triangles[1][1] = 2;
        triangles[1][2] = 3;


    }

    RaySquareIntersection intersect(const Ray &ray) const {
        RaySquareIntersection intersection;

        //TODO calculer l'intersection rayon quad

        return intersection;
    }
};
#endif // SQUARE_H
