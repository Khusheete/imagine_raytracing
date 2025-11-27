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


#pragma once


#include <filesystem>
#include <vector>
#include <cfloat>
#include <cstdint>

#include "thirdparty/glad/include/glad/glad.h"

#include "thirdparty/kmath/matrix.hpp"
#include "thirdparty/kmath/vector.hpp"

#include "ray.hpp"
#include "material.hpp"



// -------------------------------------------
// Basic Mesh class
// -------------------------------------------


struct MeshVertex {
  kmath::Vec3 position;
  kmath::Vec3 normal;
  kmath::Vec2 uv;
};


struct MeshTriangle {
  uint32_t a = 0, b = 0, c = 0;

public:
  inline unsigned int &operator[](unsigned int iv) { return *(&a + iv); }
  inline unsigned int operator[](unsigned int iv) const { return *(&a + iv); }
};




class Mesh {
public:
  Material material;

  // void load_off(const std::string & filename);
  void load_obj(const std::filesystem::path &p_path);
  void recompute_normals();
  void center_and_scale_to_unit();
  void scale_unit();

  void build_arrays();

  void scale(const kmath::Vec3 &p_scale);
  void rotate_x(const float p_angle);
  void rotate_y(const float p_angle);
  void rotate_z(const float p_angle);
  void translate(const kmath::Vec3 &p_translation);
  void apply_transformation_matrix(const kmath::Mat3 &p_transform);

  void draw() const;

  RayTriangleIntersection intersect(const Ray &p_ray) const;

  Mesh() = default;
  Mesh(Mesh&&) = default;
  Mesh &operator=(Mesh&&) = default;
  Mesh(const Mesh&) = default;
  Mesh &operator=(const Mesh&) = default;
  ~Mesh() = default;


protected:
  void build_positions_array();
  void build_normals_array();
  void build_UVs_array();
  void build_triangles_array();


protected:
  std::vector<MeshVertex> vertices;
  std::vector<MeshTriangle> triangles;

  std::vector<float> positions_array;
  std::vector<float> normals_array;
  std::vector<float> uvs_array;
  std::vector<unsigned int> triangles_array;
};

