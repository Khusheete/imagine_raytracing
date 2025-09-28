#include "mesh.h"


#include <iostream>
#include <fstream>


void Mesh::loadOFF (const std::string & filename) {
    std::ifstream in (filename.c_str ());
    if (!in)
        exit (EXIT_FAILURE);
    std::string offString;
    unsigned int sizeV, sizeT, tmp;
    in >> offString >> sizeV >> sizeT >> tmp;
    vertices.resize(sizeV);
    triangles.resize (sizeT);
    for (unsigned int i = 0; i < sizeV; i++) {
        float x, y, z;
        in >> x >> y >> z;
        vertices[i].position = kmath::Vec3(x, y, z);
    }
    int s;
    for (unsigned int i = 0; i < sizeT; i++) {
        in >> s;
        for (unsigned int j = 0; j < 3; j++)
            in >> triangles[i][j];
    }
    in.close ();
}

void Mesh::recomputeNormals () {
    for (unsigned int i = 0; i < vertices.size (); i++)
        vertices[i].normal = kmath::Vec3 (0.0, 0.0, 0.0);
    for (unsigned int i = 0; i < triangles.size (); i++) {
        kmath::Vec3 e01 = vertices[triangles[i][1]].position -  vertices[triangles[i][0]].position;
        kmath::Vec3 e02 = vertices[triangles[i][2]].position -  vertices[triangles[i][0]].position;
        kmath::Vec3 n = kmath::normalized(kmath::cross(e01, e02));
        for (unsigned int j = 0; j < 3; j++)
            vertices[triangles[i][j]].normal += n;
    }
    for (unsigned int i = 0; i < vertices.size (); i++) {
        vertices[i].normal = normalized(vertices[i].normal);
    }
}

void Mesh::centerAndScaleToUnit() {
    kmath::Vec3 c(0,0,0);
    for  (unsigned int i = 0; i < vertices.size(); i++)
        c += vertices[i].position;
    c /= static_cast<float>(vertices.size());

    float maxD = kmath::length(vertices[0].position - c);
    for (unsigned int i = 0; i < vertices.size(); i++){
        float m = length(vertices[i].position - c);
        if (m > maxD)
            maxD = m;
    }
    for  (unsigned int i = 0; i < vertices.size(); i++)
        vertices[i].position = (vertices[i].position - c) / maxD;
}
