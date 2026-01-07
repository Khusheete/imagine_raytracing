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


#include <cstddef>
#include <memory>
#include <span>
#include <variant>


#include "geometry/ray.hpp"
#include "tp_utils/src/data_structures/stack_vector.hpp"
#include "tp_utils/src/data_structures/aabb.hpp"


class KDTree {
public:

  RayMeshIntersection intersect(const Ray &p_ray) const;
  void draw() const;

  // The lifetime of the KDTree should exced that of the data pointed by p_triangles and p_positions.
  static KDTree build_kdtree(std::span<const kmath::Vec3i> p_triangles, std::span<const kmath::Vec3> p_vertex_positions, std::span<const kmath::Vec3> p_normals, std::span<const kmath::Vec2> p_uvs);


private:
  KDTree() = default;  

private:
  constexpr static size_t MAX_ELEM_PER_LEAF = 32;


private:
  struct Node {
  public:
    struct Leaf {
      tputils::StackVector<size_t, MAX_ELEM_PER_LEAF> elements;

    };

    struct Subdivision {
      std::unique_ptr<Node> le;
      std::unique_ptr<Node> ge;

      float value;

      enum class Axis : int {
        X = 0, Y = 1, Z = 2, AXIS_MAX = 3
      } axis;
    };



  public:
    std::variant<Leaf, Subdivision> data;
  };


private:
  static std::pair<tputils::AABB, tputils::AABB> _cut_aabb(const tputils::AABB &p_parent, const float p_value, const Node::Subdivision::Axis p_axis);
  static inline float _get_component(const kmath::Vec3 &p_vector, const Node::Subdivision::Axis p_axis) {
    switch (p_axis) {
    break;case Node::Subdivision::Axis::X: return p_vector.x;
    break;case Node::Subdivision::Axis::Y: return p_vector.y;
    break;case Node::Subdivision::Axis::Z: return p_vector.z;
    break;case Node::Subdivision::Axis::AXIS_MAX: break;
    }
    return 0.0f;
  };



private:
  Node root;
  tputils::AABB aabb;
  std::span<const kmath::Vec3i> triangle_elements;
  std::span<const kmath::Vec3> vertex_positions;
  std::span<const kmath::Vec3> vertex_normals;
  std::span<const kmath::Vec2> vertex_uvs;
};
