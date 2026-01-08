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


#include "acceleration_structures.hpp"


#include <algorithm>
#include <cfloat>
#include <functional>
#include <limits>
#include <memory>
#include <span>
#include <stack>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include "geometry/ray.hpp"
#include "geometry/triangle.hpp"
#include "thirdparty/kmath/color.hpp"
#include "thirdparty/kmath/euclidian_flat_3d.hpp"
#include "thirdparty/kmath/vector.hpp"
#include "tp_utils/src/debug.hpp"
#include "tp_utils/src/rendering/immediate_geometry.hpp"
#include "utils/random.hpp"
#include "utils/renderer.hpp"


#warning remove
#include "thirdparty/kmath/print.hpp"


using namespace kmath;
using namespace tputils;


void draw_aabb(const AABB &p_aabb) {
  Renderer *rd = Renderer::get_singleton();

  tputils::ImmediateGeometry &imgeo = rd->immediate_geometry();

  imgeo.begin(tputils::ImmediateGeometry::Mode::LINES, rd->get_default_buffer_layout());

  imgeo.push_vec3(Vec3(p_aabb.begin.x, p_aabb.begin.y, p_aabb.begin.z));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  imgeo.push_vec3(Vec3(p_aabb.end.x  , p_aabb.begin.y, p_aabb.begin.z));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);

  imgeo.push_vec3(Vec3(p_aabb.begin.x, p_aabb.begin.y, p_aabb.begin.z));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  imgeo.push_vec3(Vec3(p_aabb.begin.x, p_aabb.end.y  , p_aabb.begin.z));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);

  imgeo.push_vec3(Vec3(p_aabb.begin.x, p_aabb.begin.y, p_aabb.begin.z));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  imgeo.push_vec3(Vec3(p_aabb.begin.x, p_aabb.begin.y, p_aabb.end.z  ));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  // =
  imgeo.push_vec3(Vec3(p_aabb.end.x  , p_aabb.end.y  , p_aabb.end.z  ));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  imgeo.push_vec3(Vec3(p_aabb.begin.x, p_aabb.end.y  , p_aabb.end.z  ));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);

  imgeo.push_vec3(Vec3(p_aabb.end.x  , p_aabb.end.y  , p_aabb.end.z  ));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  imgeo.push_vec3(Vec3(p_aabb.end.x  , p_aabb.begin.y, p_aabb.end.z  ));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);

  imgeo.push_vec3(Vec3(p_aabb.end.x  , p_aabb.end.y  , p_aabb.end.z  ));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  imgeo.push_vec3(Vec3(p_aabb.end.x  , p_aabb.end.y  , p_aabb.begin.z));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  // =
  imgeo.push_vec3(Vec3(p_aabb.end.x  , p_aabb.begin.y, p_aabb.begin.z));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  imgeo.push_vec3(Vec3(p_aabb.end.x  , p_aabb.end.y  , p_aabb.begin.z));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);

  imgeo.push_vec3(Vec3(p_aabb.end.x  , p_aabb.begin.y, p_aabb.begin.z));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  imgeo.push_vec3(Vec3(p_aabb.end.x  , p_aabb.begin.y, p_aabb.end.z  ));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  // =
  imgeo.push_vec3(Vec3(p_aabb.begin.x, p_aabb.end.y  , p_aabb.begin.z));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  imgeo.push_vec3(Vec3(p_aabb.end.x  , p_aabb.end.y  , p_aabb.begin.z));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);

  imgeo.push_vec3(Vec3(p_aabb.begin.x, p_aabb.end.y  , p_aabb.begin.z));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  imgeo.push_vec3(Vec3(p_aabb.begin.x, p_aabb.end.y  , p_aabb.end.z  ));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  // =
  imgeo.push_vec3(Vec3(p_aabb.begin.x, p_aabb.begin.y, p_aabb.end.z  ));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  imgeo.push_vec3(Vec3(p_aabb.end.x  , p_aabb.begin.y, p_aabb.end.z  ));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);

  imgeo.push_vec3(Vec3(p_aabb.begin.x, p_aabb.begin.y, p_aabb.end.z  ));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  imgeo.push_vec3(Vec3(p_aabb.begin.x, p_aabb.end.y  , p_aabb.end.z  ));
  imgeo.push_vec3(Vec3::ZERO);
  imgeo.push_vec2(Vec2::ZERO);
  
  imgeo.end();
}


bool is_ray_aabb_intersection(const AABB &p_aabb, const Ray &p_ray) {
  if (
    (p_ray.direction.x >= 0.0f && p_aabb.end.x <= p_ray.origin.x)
    || (p_ray.direction.x <= 0.0f && p_aabb.begin.x >= p_ray.origin.x)
    || (p_ray.direction.y >= 0.0f && p_aabb.end.y <= p_ray.origin.y)
    || (p_ray.direction.y <= 0.0f && p_aabb.begin.y >= p_ray.origin.y)
    || (p_ray.direction.z >= 0.0f && p_aabb.end.z <= p_ray.origin.z)
    || (p_ray.direction.z <= 0.0f && p_aabb.begin.z >= p_ray.origin.z)
  ) {
    return false;
  }

  auto is_in_rectangle = [](const Vec2 &p_point, const Vec2 &p_min, const Vec2 &p_max) -> bool {
    return p_min.x <= p_point.x && p_point.x <= p_max.x && p_min.y <= p_point.y && p_point.y <= p_max.y;
  };
  
  const Line3 line = Line3::line(p_ray.direction, p_ray.origin);
  
  const float x = (
   p_ray.origin.x <= p_aabb.begin.x
   || (p_ray.origin.x <= p_aabb.end.x && p_ray.direction.x < 0.0f)
  )? p_aabb.begin.x : p_aabb.end.x;
  const float y = (
   p_ray.origin.y <= p_aabb.begin.y
   || (p_ray.origin.y <= p_aabb.end.y && p_ray.direction.y < 0.0f)
  )? p_aabb.begin.y : p_aabb.end.y;
  const float z = (
   p_ray.origin.z <= p_aabb.begin.z
   || (p_ray.origin.z <= p_aabb.end.z && p_ray.direction.z < 0.0f)
  )? p_aabb.begin.z : p_aabb.end.z;

  const Vec3 x_intersect = as_vector(meet(line, Plane3::plane(Vec3::X, x)));
  const Vec3 y_intersect = as_vector(meet(line, Plane3::plane(Vec3::Y, y)));
  const Vec3 z_intersect = as_vector(meet(line, Plane3::plane(Vec3::Z, z)));

  return (
    is_in_rectangle(x_intersect.yz(), p_aabb.begin.yz(), p_aabb.end.yz())
    || is_in_rectangle(y_intersect.zx(), p_aabb.begin.zx(), p_aabb.end.zx())
    || is_in_rectangle(z_intersect.xy(), p_aabb.begin.xy(), p_aabb.end.xy())
  );
}


std::pair<AABB, AABB> KDTree::_cut_aabb(const AABB &p_parent, const float p_value, const Node::Subdivision::Axis p_axis) {
  switch (p_axis) {
  case Node::Subdivision::Axis::X:
    return {
      AABB(p_parent.begin, Vec3(p_value, p_parent.end.y, p_parent.end.z)),
      AABB(Vec3(p_value, p_parent.begin.y, p_parent.begin.z), p_parent.end),
    };
  case Node::Subdivision::Axis::Y:
    return {
      AABB(p_parent.begin, Vec3(p_parent.end.x, p_value, p_parent.end.z)),
      AABB(Vec3(p_parent.begin.x, p_value, p_parent.begin.z), p_parent.end),
    };
  case Node::Subdivision::Axis::Z:
    return {
      AABB(p_parent.begin, Vec3(p_parent.end.x, p_parent.end.y, p_value)),
      AABB(Vec3(p_parent.begin.x, p_parent.begin.y, p_value), p_parent.end),
    };
  case Node::Subdivision::Axis::AXIS_MAX:
    break;
  }
  return {};
}


KDTree KDTree::build_kdtree(std::span<const Vec3i> p_triangles, std::span<const Vec3> p_positions, std::span<const kmath::Vec3> p_normals, std::span<const kmath::Vec2> p_uvs) {
  const Vec3 EPSILON = 0.001f * Vec3::ONE;

  KDTree tree;
  tree.triangle_elements = p_triangles;
  tree.vertex_positions = p_positions;
  tree.vertex_normals = p_normals;
  tree.vertex_uvs = p_uvs;

  auto minf = [](const float a, const float b) -> float {
    return std::min(a, b);
  };
  auto maxf = [](const float a, const float b) -> float {
    return std::max(a, b);
  };

  { // Build the mesh's AABB
    Vec3 minimum = Vec3::INF;
    Vec3 maximum = -Vec3::INF;

    for (const Vec3 &pos : p_positions) {
      minimum = apply(pos, minimum, minf);
      maximum = apply(pos, maximum, maxf);
    }

    tree.aabb = AABB(minimum - EPSILON, maximum + EPSILON);
  }

  std::vector<Vec3> tri_positions(p_triangles.size());
  { // Get an idea of triangle positions
    constexpr float THIRD = 1.0f / 3.0f;

    for (size_t i = 0; i < p_triangles.size(); i++) {
      const Vec3i &tri = p_triangles[i];
      tri_positions[i] = (p_positions[tri.x] + p_positions[tri.y] + p_positions[tri.z]) * THIRD;
    }
  }

  std::vector<AABB> triangles_aabb(p_triangles.size());
  { // Get AABBs for each triangles
    for (size_t i = 0; i < p_triangles.size(); i++) {
      AABB &aabb = triangles_aabb[i];
      const Vec3i &tri = p_triangles[i];

      aabb.begin = p_positions[tri.x];
      aabb.end = p_positions[tri.x];

      aabb.begin = apply(aabb.begin, p_positions[tri.y], minf);
      aabb.end = apply(aabb.end, p_positions[tri.y], maxf);

      aabb.begin = apply(aabb.begin, p_positions[tri.z], minf);
      aabb.end = apply(aabb.end, p_positions[tri.z], maxf);
    }
  }


  // Helper functions
  auto sort_triangles = [&](std::span<size_t> p_triangle_indices, const Node::Subdivision::Axis p_division_axis) -> void {
    switch (p_division_axis) {
    break; case Node::Subdivision::Axis::X:
      std::sort(
        p_triangle_indices.begin(),
        p_triangle_indices.end(),
        [&](const size_t a, const size_t b) -> bool { return tri_positions[a].x < tri_positions[b].x;
      });
    break; case Node::Subdivision::Axis::Y:
      std::sort(
        p_triangle_indices.begin(),
        p_triangle_indices.end(),
        [&](const size_t a, const size_t b) -> bool { return tri_positions[a].y < tri_positions[b].y;
      });
    break; case Node::Subdivision::Axis::Z:
      std::sort(
        p_triangle_indices.begin(),
        p_triangle_indices.end(),
        [&](const size_t a, const size_t b) -> bool { return tri_positions[a].z < tri_positions[b].z;
      });
    break; case Node::Subdivision::Axis::AXIS_MAX: return; // Do nothing
    }
  };

  auto next_axis = [&](const Node::Subdivision::Axis p_division_axis) -> Node::Subdivision::Axis {
    using Axis = Node::Subdivision::Axis;
    return static_cast<Axis>((static_cast<int>(p_division_axis) + 1) % static_cast<int>(Axis::AXIS_MAX));
  };

  // Recursive function to build the kdtree
  std::function<Node(std::span<size_t>, const Node::Subdivision::Axis)> build_node =
    [&](std::span<size_t> p_triangle_indices, const Node::Subdivision::Axis p_division_axis) -> Node {
      // When the condition to stop building the kdtree is met, create a leaf
      if (p_triangle_indices.size() <= MAX_ELEM_PER_LEAF) {
        Node::Leaf leaf{};
        for (const size_t &idx : p_triangle_indices) {
          leaf.elements.push_back(idx);
        }
        return Node(std::move(leaf));
      }

      // Build the space subdivision
      Node::Subdivision sub{};
      sub.axis = p_division_axis;

      // Split the triangles using the median
      sort_triangles(p_triangle_indices, p_division_axis);
      if (p_triangle_indices.size() % 2 == 0) {
        const size_t mid = p_triangle_indices.size() / 2;
        const Vec3 a = tri_positions[p_triangle_indices[mid]];
        const Vec3 b = tri_positions[p_triangle_indices[mid + 1]];
        sub.value = _get_component(0.5f * (a + b), p_division_axis);
      } else {
        const size_t mid = p_triangle_indices.size() / 2;
        sub.value = _get_component(tri_positions[p_triangle_indices[mid]], p_division_axis);
      }

      std::vector<size_t> less_triangle_indices;
      std::vector<size_t> greater_triangles_indices;

      for (const size_t tri_index : p_triangle_indices) {
        if (_get_component(triangles_aabb[tri_index].begin, p_division_axis) <= sub.value) {
          less_triangle_indices.push_back(tri_index);
        }
        if (_get_component(triangles_aabb[tri_index].end, p_division_axis) >= sub.value) {
          greater_triangles_indices.push_back(tri_index);
        }
      }

      Node::Subdivision::Axis next_division_axis = next_axis(p_division_axis);

      if (less_triangle_indices.size() == p_triangle_indices.size() || greater_triangles_indices.size() == p_triangle_indices.size()) {
        LOG_WARNING("MESH WITH TOO MANY TRIANGLES ON THE SAME PLANE, CANNOT BUILD KDTREE");
      }
      
      sub.le = std::make_unique<Node>(build_node(
        std::span<size_t>(less_triangle_indices), next_division_axis
      ));
      sub.ge = std::make_unique<Node>(build_node(
        std::span<size_t>(greater_triangles_indices), next_division_axis
      ));

      return Node(std::move(sub));
    };


  // Build the tree
  std::vector<size_t> triangle_indices(p_triangles.size());
  for (size_t i = 0; i < p_triangles.size(); i++) {
    triangle_indices[i] = i;
  }
  tree.root = build_node(triangle_indices, Node::Subdivision::Axis::X);
  
  return tree;
}


RayMeshIntersection KDTree::intersect(const Ray &p_ray) const {
  RayMeshIntersection closest_intersection;
  closest_intersection.exists = false;
  closest_intersection.distance = std::numeric_limits<float>::max();

  // We know that we don't need to traverse the kdtree, the ray goes outside
  if (!is_ray_aabb_intersection(aabb, p_ray)) {
    return closest_intersection;
  }

  Line3 line = Line3::line(p_ray.origin, p_ray.direction);

  // Helper functions
  auto get_plane_intersection_distance = [&](const Plane3 &p_plane) -> float {
    return dot(as_vector(meet(line, p_plane)), p_ray.direction);
  };

  auto get_full_aabb_intersect = [&](const AABB &p_aabb) -> std::pair<float, float> {
    const Plane3 pxb = Plane3::plane(Vec3::X, p_aabb.begin.x);
    const Plane3 pxe = Plane3::plane(Vec3::X, p_aabb.end.x);
    const Plane3 pyb = Plane3::plane(Vec3::Y, p_aabb.begin.y);
    const Plane3 pye = Plane3::plane(Vec3::Y, p_aabb.end.y);
    const Plane3 pzb = Plane3::plane(Vec3::Z, p_aabb.begin.z);
    const Plane3 pze = Plane3::plane(Vec3::Z, p_aabb.end.z);

    const float txb = get_plane_intersection_distance(pxb);
    const float txe = get_plane_intersection_distance(pxe);
    const float tyb = get_plane_intersection_distance(pyb);
    const float tye = get_plane_intersection_distance(pye);
    const float tzb = get_plane_intersection_distance(pzb);
    const float tze = get_plane_intersection_distance(pze);

    return {
      std::max({std::min(txb, txe), std::min(tyb, tye), std::min(tzb, tze)}),
      std::min({std::max(txb, txe), std::max(tyb, tye), std::max(tzb, tze)}),
    };
  };

  auto get_bisection_planes = [](const float p_value, const Node::Subdivision::Axis p_axis) -> Plane3 {
    switch (p_axis) {
    break;case Node::Subdivision::Axis::X: Plane3::plane(Vec3::X, p_value);
    break;case Node::Subdivision::Axis::Y: Plane3::plane(Vec3::Y, p_value);
    break;case Node::Subdivision::Axis::Z: Plane3::plane(Vec3::Z, p_value);
    break;case Node::Subdivision::Axis::AXIS_MAX: break;
    }
    return {};
  };
  
  // Structure traversal
  std::stack<std::tuple<const Node*, AABB>> to_explore;
  to_explore.push({&root, aabb});

  while (!to_explore.empty()) {
    const auto [node, parent_aabb] = to_explore.top();
    to_explore.pop();

    if (std::holds_alternative<Node::Leaf>(node->data)) {
      const Node::Leaf &leaf = std::get<Node::Leaf>(node->data);

      // Perform an intersection with every element of the leaf
      for (const size_t tri_index : leaf.elements) {
        const Vec3i element = triangle_elements[tri_index];
        const Triangle tri{{
         vertex_positions[element.x],
         vertex_positions[element.y],
         vertex_positions[element.z]
        }};

        const auto intersection_opt = get_intersection(p_ray, tri);
        if (!intersection_opt.has_value()) continue;
        const RayTriangleIntersection intersection = intersection_opt.value();

        if (intersection.distance >= closest_intersection.distance) continue;

        const Vec3 normal = normalized(
          intersection.barycentric.x * vertex_normals[element.x]
          + intersection.barycentric.y * vertex_normals[element.x]
          + intersection.barycentric.z * vertex_normals[element.x]
        );

        const Vec2 uv = (
          intersection.barycentric.x * vertex_uvs[element.x]
          + intersection.barycentric.y * vertex_uvs[element.x]
          + intersection.barycentric.z * vertex_uvs[element.x]
        );

        closest_intersection.position = intersection.position;
        closest_intersection.distance = intersection.distance;
        closest_intersection.normal = normal;
        closest_intersection.uv = uv;
        closest_intersection.barycentric = intersection.barycentric;
      }
    } else {
      const Node::Subdivision &sub = std::get<Node::Subdivision>(node->data);
      const auto [le_aabb, ge_aabb] = _cut_aabb(parent_aabb, sub.value, sub.axis);
      const auto [t_min, t_max] = get_full_aabb_intersect(parent_aabb);
      const Plane3 bis_plane = get_bisection_planes(sub.value, sub.axis);
      const float t_bis = get_plane_intersection_distance(bis_plane);

      if (t_min < t_bis) {
        to_explore.push({sub.le.get(), le_aabb});
      }
      if (t_max > t_bis) {
        to_explore.push({sub.ge.get(), ge_aabb});
      }
    }
  }

  closest_intersection.exists = (closest_intersection.distance != FLT_MAX);
  return closest_intersection;
}


void KDTree::draw() const {
  Renderer *rd = Renderer::get_singleton();
  tputils::ImmediateGeometry &imgeo = rd->immediate_geometry();
  
  std::stack<std::tuple<const Node*, AABB>> to_explore;
  to_explore.push({&root, aabb});

  while (!to_explore.empty()) {
    const auto [node, parent_aabb] = to_explore.top();
    to_explore.pop();

    if (std::holds_alternative<Node::Leaf>(node->data)) {
      const Node::Leaf &leaf = std::get<Node::Leaf>(node->data);
      const Lrgb color = Lrgb(
        0.5f + 0.5f * spatial_random(parent_aabb.begin.x + parent_aabb.end.y),
        0.5f + 0.5f * spatial_random(parent_aabb.begin.y + parent_aabb.end.z),
        0.5f + 0.5f * spatial_random(parent_aabb.begin.z + parent_aabb.end.x)
      );
      rd->set_color(color);

      imgeo.begin(ImmediateGeometry::Mode::POINTS, rd->get_default_buffer_layout());
      for (const size_t tri_index : leaf.elements) {
        const Vec3i tri = triangle_elements[tri_index];
        const Vec3 pos = 0.3333f * (
          vertex_positions[tri.x] + vertex_positions[tri.y] + vertex_positions[tri.z]
        );
        imgeo.push_vec3(pos);
        imgeo.push_vec3(Vec3::ZERO);
        imgeo.push_vec2(Vec2::ZERO);
      }
      imgeo.end();

      draw_aabb(parent_aabb);
    } else {
      const Node::Subdivision &sub = std::get<Node::Subdivision>(node->data);
      const auto [le_aabb, ge_aabb] = _cut_aabb(parent_aabb, sub.value, sub.axis);
      to_explore.push({sub.le.get(), le_aabb});
      to_explore.push({sub.ge.get(), ge_aabb});
    }
  }
}

