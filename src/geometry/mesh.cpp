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
#include "geometry/triangle.hpp"
#include "thirdparty/kmath/matrix.hpp"
#include "thirdparty/kmath/vector.hpp"
#include "tp_utils/src/model_loaders/wavefront_object.hpp"
#include "tp_utils/src/rendering/immediate_geometry.hpp"
#include "utils/renderer.hpp"


#include <cfloat>
#include <filesystem>

#include <GL/gl.h>
#include <unordered_map>


void Mesh::load_obj(const std::filesystem::path &p_path) {
  tputils::WavefrontMesh wavefront = tputils::WavefrontMesh::load(p_path);

  // Create vertex data suited for single index buffer for positions, normals and uvs
  positions_array.reserve(sizeof(wavefront.positions[0]) * wavefront.positions.size() / sizeof(float));
  normals_array.reserve(sizeof(wavefront.normals[0]) * wavefront.positions.size() / sizeof(float));
  uvs_array.reserve(sizeof(wavefront.uvs[0]) * wavefront.positions.size() / sizeof(float));
  uint32_t vertex_count = 0;

  std::unordered_map<uint64_t, uint32_t> index_map;

  size_t object_index_buffer_size = 0;
  std::vector<uint32_t> object_indices;

  for (auto element : wavefront.objects) {
    tputils::WavefrontObject object = element.second;

    object_indices.reserve(object.position_indices.size());
    object_index_buffer_size = 0;

    // Build index buffer
    for (size_t i = 0; i < object.position_indices.size(); i++) {
      uint64_t position_index = object.position_indices[i];
      uint64_t normal_index = object.normal_indices[i];
      uint64_t uv_index = object.uv_indices[i];

      // Assume that indices fit in a 16 bit number
      uint64_t index_id = position_index + (normal_index << 0x10) + (uv_index << 0x20);

      auto mapped_index = index_map.find(index_id);
      if (mapped_index == index_map.end()) {
        // Add the new vertex
        kmath::Vec3 position = wavefront.positions[position_index];
        kmath::Vec3 normal = wavefront.normals[normal_index];
        kmath::Vec2 uv = wavefront.uvs[uv_index];
        positions_array.push_back(position.x);
        positions_array.push_back(position.y);
        positions_array.push_back(position.z);
        normals_array.push_back(normal.x);
        normals_array.push_back(normal.y);
        normals_array.push_back(normal.z);
        uvs_array.push_back(uv.x);
        uvs_array.push_back(uv.y);

        if (object_indices.size() > object_index_buffer_size) {
          object_indices[object_index_buffer_size] = vertex_count;
        } else {
          object_indices.push_back(vertex_count);
        }
        object_index_buffer_size += 1;

        index_map[index_id] = vertex_count;
        vertex_count += 1;
      } else {
        if (object_indices.size() > object_index_buffer_size) {
          object_indices[object_index_buffer_size] = mapped_index->second;
        } else {
          object_indices.push_back(mapped_index->second);
        }
        object_index_buffer_size += 1;
      }
    }

    // Create surfaces
    for (size_t i = 0; i < object_index_buffer_size; i++) {
      triangles_array.push_back(object_indices[i]);
    }
  }
}


// void Mesh::load_off(const std::string &filename) {
//   std::ifstream in(filename.c_str());
//   ASSERT_FATAL_ERROR(in, "Cannot open file " << filename);
//   std::string offString;
//   unsigned int sizeV, sizeT, tmp;
//   in >> offString >> sizeV >> sizeT >> tmp;
//   vertices.resize(sizeV);
//   triangles.resize(sizeT);
//   for (unsigned int i = 0; i < sizeV; i++) {
//     float x, y, z;
//     in >> x >> y >> z;
//     vertices[i].position = kmath::Vec3(x, y, z);
//   }
//   int s;
//   for (unsigned int i = 0; i < sizeT; i++) {
//     in >> s;
//     for (unsigned int j = 0; j < 3; j++)
//       in >> triangles[i][j];
//   }
//   in.close();
// }


void Mesh::recompute_normals() {
  for (unsigned int i = 0; i < get_vertex_count(); i++)
    get_normal(i) = kmath::Vec3(0.0, 0.0, 0.0);
  for (unsigned int i = 0; i < get_triangle_count(); i++) {
    kmath::Vec3 e01 = get_position(triangles_array[3 * i + 1]) - get_position(triangles_array[3 * i + 0]);
    kmath::Vec3 e02 = get_position(triangles_array[3 * i + 2]) - get_position(triangles_array[3 * i + 0]);
    kmath::Vec3 n = kmath::cross(e01, e02);
    for (unsigned int j = 0; j < 3; j++)
      get_normal(triangles_array[3 * i + j]) += n;
  }
  for (unsigned int i = 0; i < get_vertex_count(); i++) {
    get_normal(i) = normalized(get_normal(i));
  }
}


// void Mesh::center_and_scale_to_unit() {
//   kmath::Vec3 c(0,0,0);
//   for (unsigned int i = 0; i < vertices.size(); i++)
//     c += vertices[i].position;
//   c /= static_cast<float>(vertices.size());

//   float maxD = kmath::length(vertices[0].position - c);
//   for (unsigned int i = 0; i < vertices.size(); i++){
//     float m = length(vertices[i].position - c);
//     if (m > maxD)
//       maxD = m;
//   }
//   for (unsigned int i = 0; i < vertices.size(); i++)
//     vertices[i].position = (vertices[i].position - c) / maxD;
// }


void Mesh::translate(const kmath::Vec3 &p_translation) {
  for (unsigned int v = 0; v < get_vertex_count(); ++v) {
    get_position(v) += p_translation;
  }
}


void Mesh::apply_transformation_matrix(const kmath::Mat3 &p_transform) {
  for (unsigned int v = 0 ; v < get_vertex_count(); ++v) {
    get_position(v) = p_transform * get_position(v);
  }
  recompute_normals();
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
  // GLfloat material_color[4] = {material.diffuse_material.x,
  //                              material.diffuse_material.y,
  //                              material.diffuse_material.z,
  //                              1.0};
  // GLfloat material_specular[4] = {material.specular_material.x,
  //                                 material.specular_material.y,
  //                                 material.specular_material.z,
  //                                 1.0};
  // GLfloat material_ambient[4] = {material.diffuse_material.x,
  //                                material.diffuse_material.y,
  //                                material.diffuse_material.z,
  //                                1.0};

  // glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
  // glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_color);
  // glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
  // glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material.shininess);
  // glEnableClientState(GL_VERTEX_ARRAY) ;
  // glEnableClientState (GL_NORMAL_ARRAY);
  // glNormalPointer(GL_FLOAT, 3 * sizeof(float), (GLvoid*)(normals_array.data()));
  // glVertexPointer(3, GL_FLOAT, 3 * sizeof(float) , (GLvoid*)(positions_array.data()));
  // glDrawElements(GL_TRIANGLES, triangles_array.size(), GL_UNSIGNED_INT, (GLvoid*)(triangles_array.data()));

  Renderer *rd = Renderer::get_singleton();
  rd->set_model_matrix(kmath::Mat4::IDENTITY);
  
  tputils::ImmediateGeometry &imgeo = rd->immediate_geometry();

  imgeo.begin(tputils::ImmediateGeometry::Mode::TRIANGLES, rd->get_default_buffer_layout());
  for (const uint32_t &index : triangles_array) {
    const kmath::Vec3 position{
      positions_array[3 * index + 0],
      positions_array[3 * index + 1],
      positions_array[3 * index + 2],
    };
    imgeo.push_vec3(position);
    const kmath::Vec3 normal{
      normals_array[3 * index + 0],
      normals_array[3 * index + 1],
      normals_array[3 * index + 2],
    };
    imgeo.push_vec3(normal);
    const kmath::Vec2 uv{
      uvs_array[2 * index + 0],
      uvs_array[2 * index + 1],
    };
    imgeo.push_vec2(uv);
  }
  imgeo.end();
}


RayMeshIntersection Mesh::intersect(const Ray &p_ray) const {
  RayMeshIntersection closest_intersection;
  closest_intersection.distance = FLT_MAX;

  for (size_t triangle_index = 0; triangle_index < triangles_array.size(); triangle_index += 3) {
    const uint32_t index_a = triangles_array[triangle_index + 0];
    const kmath::Vec3 point_a{
      positions_array[3 * index_a + 0],
      positions_array[3 * index_a + 1],
      positions_array[3 * index_a + 2],
    };
    const uint32_t index_b = triangles_array[triangle_index + 1];
    const kmath::Vec3 point_b{
      positions_array[3 * index_b + 0],
      positions_array[3 * index_b + 1],
      positions_array[3 * index_b + 2],
    };
    const uint32_t index_c = triangles_array[triangle_index + 2];
    const kmath::Vec3 point_c{
      positions_array[3 * index_c + 0],
      positions_array[3 * index_c + 1],
      positions_array[3 * index_c + 2],
    };
    const Triangle tri{{
      point_a, point_b, point_c
    }};
    const auto intersection_opt = get_intersection(p_ray, tri);
    if (!intersection_opt.has_value()) continue;
    const RayTriangleIntersection intersection = intersection_opt.value();
    
    if (intersection.distance >= closest_intersection.distance) continue;

    const kmath::Vec3 normal_a{
      normals_array[3 * index_a + 0],
      normals_array[3 * index_a + 1],
      normals_array[3 * index_a + 2],
    };
    const kmath::Vec3 normal_b{
      normals_array[3 * index_b + 0],
      normals_array[3 * index_b + 1],
      normals_array[3 * index_b + 2],
    };
    const kmath::Vec3 normal_c{
      normals_array[3 * index_c + 0],
      normals_array[3 * index_c + 1],
      normals_array[3 * index_c + 2],
    };

    const kmath::Vec2 uv_a{
      uvs_array[2 * index_a + 0],
      uvs_array[2 * index_a + 1],
    };
    const kmath::Vec2 uv_b{
      uvs_array[2 * index_b + 0],
      uvs_array[2 * index_b + 1],
    };
    const kmath::Vec2 uv_c{
      uvs_array[2 * index_c + 0],
      uvs_array[2 * index_c + 1],
    };

    closest_intersection.position = intersection.position;
    closest_intersection.distance = intersection.distance;
    closest_intersection.normal = kmath::normalized(
      intersection.barycentric.x * normal_a
      + intersection.barycentric.y * normal_b
      + intersection.barycentric.z * normal_c
    );
    closest_intersection.uv = (
      intersection.barycentric.x * uv_a
      + intersection.barycentric.y * uv_b
      + intersection.barycentric.z * uv_c
    );
    closest_intersection.barycentric = intersection.barycentric;
  }

  closest_intersection.exists = (closest_intersection.distance != FLT_MAX);
  return closest_intersection;
}
