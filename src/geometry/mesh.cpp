/* ------------------------------------------------------------------------------------------------------------------ *
*                                                                                                                     *
*                                                                                                                     *
*                                                /\                    ^__                                            *
*                                               /#*\  /\              /##@>                                           *
*                                              <#* *> \/         _^_  \\    _^_                                       *
*                                               \##/            /###\ \è\  /###\                                      *
*                                                \/ /\         /#####n/xx\n#####\                                     *
*                   Ferdinand                       \/         \###^##xXXx##^###/                                     *
*                        Souchet                                \#/ V¨\xx/¨V \#/                                      *
*                     (aka. @Khusheete)                          V     \c\    V                                       *
*                                                                       //                                            *
*                                                                     \o/                                             *
*             ferdinand.souchet@etu.umontpellier.fr                    v                                              *
*                                                                                                                     *
*                                                                                                                     *
*                                                                                                                     *
*                                                                                                                     *
* Copyright 2025 Ferdinand Souchet (aka. @Khusheete)                                                                  *
*                                                                                                                     *
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated        *
* documentation files (the “Software”), to deal in the Software without restriction, including without limitation the *
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to     *
* permit persons to whom the Software is furnished to do so, subject to the following conditions:                     *
*                                                                                                                     *
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of    *
* the Software.                                                                                                       *
*                                                                                                                     *
* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO    *
* THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE      *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, *
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE      *
* SOFTWARE.                                                                                                           *
*                                                                                                                     *
* ------------------------------------------------------------------------------------------------------------------ */


#include "mesh.hpp"
#include "geometry/ray.hpp"


#include <iostream>
#include <fstream>

#include <GL/gl.h>


void Mesh::build_arrays() {
  recompute_normals();
  build_positions_array();
  build_normals_array();
  build_UVs_array();
  build_triangles_array();
}


void Mesh::build_positions_array() {
  positions_array.resize(3 * vertices.size());
  for( unsigned int v = 0 ; v < vertices.size() ; ++v ) {
    positions_array[3*v + 0] = vertices[v].position.x;
    positions_array[3*v + 1] = vertices[v].position.y;
    positions_array[3*v + 2] = vertices[v].position.z;
  }
}


void Mesh::build_normals_array() {
  normals_array.resize(3 * vertices.size());
  for( unsigned int v = 0 ; v < vertices.size() ; ++v ) {
    normals_array[3*v + 0] = vertices[v].normal.x;
    normals_array[3*v + 1] = vertices[v].normal.y;
    normals_array[3*v + 2] = vertices[v].normal.z;
  }
}


void Mesh::build_UVs_array() {
  uvs_array.resize(2 * vertices.size());
  for( unsigned int vert = 0 ; vert < vertices.size() ; ++vert ) {
    uvs_array[2*vert + 0] = vertices[vert].uv.x;
    uvs_array[2*vert + 1] = vertices[vert].uv.y;
  }
}


void Mesh::build_triangles_array() {
  triangles_array.resize(3 * triangles.size());
  for( unsigned int t = 0 ; t < triangles.size() ; ++t ) {
    triangles_array[3*t + 0] = triangles[t][0];
    triangles_array[3*t + 1] = triangles[t][1];
    triangles_array[3*t + 2] = triangles[t][2];
  }
}


void Mesh::load_off(const std::string & filename) {
  std::ifstream in(filename.c_str());
  if (!in)
    exit(EXIT_FAILURE);
  std::string offString;
  unsigned int sizeV, sizeT, tmp;
  in >> offString >> sizeV >> sizeT >> tmp;
  vertices.resize(sizeV);
  triangles.resize(sizeT);
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
  in.close();
}


void Mesh::recompute_normals() {
  for (unsigned int i = 0; i < vertices.size (); i++)
    vertices[i].normal = kmath::Vec3(0.0, 0.0, 0.0);
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


void Mesh::center_and_scale_to_unit() {
  kmath::Vec3 c(0,0,0);
  for (unsigned int i = 0; i < vertices.size(); i++)
    c += vertices[i].position;
  c /= static_cast<float>(vertices.size());

  float maxD = kmath::length(vertices[0].position - c);
  for (unsigned int i = 0; i < vertices.size(); i++){
    float m = length(vertices[i].position - c);
    if (m > maxD)
      maxD = m;
  }
  for (unsigned int i = 0; i < vertices.size(); i++)
    vertices[i].position = (vertices[i].position - c) / maxD;
}


void Mesh::translate(const kmath::Vec3 &p_translation) {
  for (unsigned int v = 0; v < vertices.size(); ++v) {
    vertices[v].position += p_translation;
  }
}


void Mesh::apply_transformation_matrix(const kmath::Mat3 &p_transform) {
  for (unsigned int v = 0 ; v < vertices.size(); ++v) {
    vertices[v].position = p_transform * vertices[v].position;
  }
  //    recomputeNormals();
  //    build_positions_array();
  //    build_normals_array();
}


void Mesh::scale(const kmath::Vec3 &p_scale){
  kmath::Mat3 scale_matrix(
    kmath::Vec3(p_scale.x, 0., 0.),
    kmath::Vec3(0., p_scale.y, 0.),
    kmath::Vec3(0., 0., p_scale.z)
  ); // Matrice de transformation de mise à l'échelle
  apply_transformation_matrix(scale_matrix);
}


void Mesh::rotate_x(const float p_angle_degrees) {
  float angle = p_angle_degrees * M_PI / 180.;
  kmath::Mat3 rotation = kmath::Mat3::x_rotation(angle);
  apply_transformation_matrix(rotation);
}


void Mesh::rotate_y(const float p_angle_degrees) {
  float angle = p_angle_degrees * M_PI / 180.;
  kmath::Mat3 rotation = kmath::Mat3::y_rotation(angle);
  apply_transformation_matrix(rotation);
}


void Mesh::rotate_z(const float p_angle_degrees) {
  float angle = p_angle_degrees * M_PI / 180.;
  kmath::Mat3 rotation = kmath::Mat3::y_rotation(angle);
  apply_transformation_matrix(rotation);
}


void Mesh::draw() const {
  if( triangles_array.size() == 0 ) return;
  GLfloat material_color[4] = {material.diffuse_material.x,
                               material.diffuse_material.y,
                               material.diffuse_material.z,
                               1.0};
  GLfloat material_specular[4] = {material.specular_material.x,
                                  material.specular_material.y,
                                  material.specular_material.z,
                                  1.0};
  GLfloat material_ambient[4] = {material.diffuse_material.x,
                                 material.diffuse_material.y,
                                 material.diffuse_material.z,
                                 1.0};

  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_color);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.shininess);
  glEnableClientState(GL_VERTEX_ARRAY) ;
  glEnableClientState (GL_NORMAL_ARRAY);
  glNormalPointer(GL_FLOAT, 3 * sizeof(float), (GLvoid*)(normals_array.data()));
  glVertexPointer(3, GL_FLOAT, 3 * sizeof(float) , (GLvoid*)(positions_array.data()));
  glDrawElements(GL_TRIANGLES, triangles_array.size(), GL_UNSIGNED_INT, (GLvoid*)(triangles_array.data()));
}


RayTriangleIntersection Mesh::intersect(const Ray &p_ray) const {
  RayTriangleIntersection closestIntersection;
  closestIntersection.distance = FLT_MAX;
  // Note :
  // Creer un objet Triangle pour chaque face
  // Vous constaterez des problemes de précision
  // solution : ajouter un facteur d'échelle lors de la création du Triangle : float triangleScaling = 1.000001;
  return closestIntersection;
}
