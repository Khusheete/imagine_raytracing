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


class Mesh {
public:
  Material material;

  // void load_off(const std::string & filename);
  void load_obj(const std::filesystem::path &p_path);
  void recompute_normals();

  void build_arrays();

  void scale(const kmath::Vec3 &p_scale);
  void rotate_x(const float p_angle);
  void rotate_y(const float p_angle);
  void rotate_z(const float p_angle);
  void translate(const kmath::Vec3 &p_translation);
  void apply_transformation_matrix(const kmath::Mat3 &p_transform);


  inline size_t get_vertex_count() {
    return positions_array.size() / 3;
  }


  inline size_t get_triangle_count() {
    return triangles_array.size() / 3;
  }
  

  inline kmath::Vec3 &get_position(const size_t p_index) {
    return reinterpret_cast<kmath::Vec3*>(positions_array.data())[p_index];
  }

  inline const kmath::Vec3 &get_position(const size_t p_index) const {
    return const_cast<Mesh*>(this)->get_position(p_index);
  }

  inline kmath::Vec3 &get_normal(const size_t p_index) {
    return reinterpret_cast<kmath::Vec3*>(normals_array.data())[p_index];
  }

  inline const kmath::Vec3 &get_normal(const size_t p_index) const {
    return const_cast<Mesh*>(this)->get_normal(p_index);
  }

  inline kmath::Vec2 &get_uv(const size_t p_index) {
    return reinterpret_cast<kmath::Vec2*>(uvs_array.data())[p_index];
  }

  inline const kmath::Vec2 &get_uv(const size_t p_index) const {
    return const_cast<Mesh*>(this)->get_uv(p_index);
  }

  void draw() const;

  RayMeshIntersection intersect(const Ray &p_ray) const;

  Mesh() = default;
  Mesh(Mesh&&) = default;
  Mesh &operator=(Mesh&&) = default;
  Mesh(const Mesh&) = default;
  Mesh &operator=(const Mesh&) = default;
  ~Mesh() = default;


protected:
  std::vector<float> positions_array;
  std::vector<float> normals_array;
  std::vector<float> uvs_array;
  std::vector<unsigned int> triangles_array;
};

