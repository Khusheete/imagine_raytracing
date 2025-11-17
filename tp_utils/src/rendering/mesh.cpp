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
#include "debug.hpp"
#include "model_loaders/wavefront_object.hpp"
#include "rendering/primitives/shader.hpp"
#include "rendering/primitives/vbuffer.hpp"

#include <cstring>
#include <span>
#include <unordered_map>


namespace tputils {
  void TriangleMesh::reserve_vertices(const size_t p_size) {
    vertex_data.reserve(p_size * get_vertex_size());
  }


  void TriangleMesh::resize_vertices(const size_t p_size) {
    vertex_data.resize(p_size * get_vertex_size());
  }


  void TriangleMesh::push_vertex(const kmath::Vec3 &p_position) {
    ASSERT_WARNING(vertex_kind == VertexKind::POSITION, "Trying to push the wrong format of vertex in TriangleMesh");
    vertex_data.push_back(p_position.x);
    vertex_data.push_back(p_position.y);
    vertex_data.push_back(p_position.z);
  }


  void TriangleMesh::push_vertex(const kmath::Vec3 &p_position, const kmath::Vec3 &p_normal) {
    ASSERT_WARNING(vertex_kind == VertexKind::POSITION_NORMAL, "Trying to push the wrong format of vertex in TriangleMesh");
    vertex_data.push_back(p_position.x);
    vertex_data.push_back(p_position.y);
    vertex_data.push_back(p_position.z);
    vertex_data.push_back(p_normal.x);
    vertex_data.push_back(p_normal.y);
    vertex_data.push_back(p_normal.z);
  }


  void TriangleMesh::push_vertex(const kmath::Vec3 &p_position, const kmath::Vec2 &p_uv) {
    ASSERT_WARNING(vertex_kind == VertexKind::POSITION_UV, "Trying to push the wrong format of vertex in TriangleMesh");
    vertex_data.push_back(p_position.x);
    vertex_data.push_back(p_position.y);
    vertex_data.push_back(p_position.z);
    vertex_data.push_back(p_uv.x);
    vertex_data.push_back(p_uv.y);
  }


  void TriangleMesh::push_vertex(const kmath::Vec3 &p_position, const kmath::Vec3 &p_normal, const kmath::Vec2 &p_uv) {
    ASSERT_WARNING(vertex_kind == VertexKind::POSITION_NORMAL_UV, "Trying to push the wrong format of vertex in TriangleMesh");
    vertex_data.push_back(p_position.x);
    vertex_data.push_back(p_position.y);
    vertex_data.push_back(p_position.z);
    vertex_data.push_back(p_normal.x);
    vertex_data.push_back(p_normal.y);
    vertex_data.push_back(p_normal.z);
    vertex_data.push_back(p_uv.x);
    vertex_data.push_back(p_uv.y);
  }


  void TriangleMesh::reserve_triangles(const size_t p_size) {
    triangles.reserve(p_size);
  }


  void TriangleMesh::resize_triangles(const size_t p_size) {
    triangles.resize(p_size);
  }


  void TriangleMesh::push_triangle(const Triangle &p_triangle) {
    triangles.push_back(p_triangle);
  }


  void TriangleMesh::gpu_upload() {
    vertex_buffer.write_data(std::span<const float>(vertex_data));
    index_buffer.write_data(std::span<const Triangle>(triangles));

    if (!vab.is_initialized()) {
      vab.initialize();
      switch (vertex_kind) {
      case VertexKind::POSITION: {
          std::array<ShaderDataType, 1> type_list = {
            ShaderDataType::Vec3,
          };
          BufferLayout layout(type_list);
        vab.add_vertex_buffer(vertex_buffer, layout);
        break;
      }
      case VertexKind::POSITION_NORMAL: {
        std::array<ShaderDataType, 2> type_list = {
          ShaderDataType::Vec3, ShaderDataType::Vec3,
        };
        BufferLayout layout(type_list);
        vab.add_vertex_buffer(vertex_buffer, layout);
        break;
      }
      case VertexKind::POSITION_UV: {
        std::array<ShaderDataType, 2> type_list = {
          ShaderDataType::Vec3, ShaderDataType::Vec2,
        };
        BufferLayout layout(type_list);
        vab.add_vertex_buffer(vertex_buffer, layout);
        break;
      }
      case VertexKind::POSITION_NORMAL_UV: {
        std::array<ShaderDataType, 3> type_list = {
          ShaderDataType::Vec3, ShaderDataType::Vec3, ShaderDataType::Vec2,
        };
        BufferLayout layout(type_list);
        vab.add_vertex_buffer(vertex_buffer, layout);
        break;
      }
      }
      vab.add_index_buffer(index_buffer);
    }
  }


  void TriangleMesh::draw() {
    vab.bind();
    glDrawElements(GL_TRIANGLES, 3 * triangles.size(), GL_UNSIGNED_INT, nullptr);
    vab.unbind();
  }


  void TriangleMesh::draw_wireframe() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
    draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }


  TriangleMesh::TriangleMesh(const VertexKind p_vertex_kind) {
    vertex_kind = p_vertex_kind;
  }


  TriangleMesh TriangleMesh::from_off_object(const OffObject &p_off) {
    TriangleMesh result(VertexKind::POSITION); // TODO: support other formats
    result.resize_vertices(p_off.positions.size());
    std::memcpy(result.vertex_data.data(), p_off.positions.data(), p_off.positions.size() * sizeof(kmath::Vec3));
    result.triangles = p_off.tris;
    result.gpu_upload();
    return result;
  }


  TriangleMesh TriangleMesh::from_wavefront_mesh(const WavefrontMesh &p_wavefront) {
    // Create returned mesh
    TriangleMesh result(VertexKind::POSITION_NORMAL_UV);

    // Create vertex data suited for single index buffer for positions, normals and uvs
    result.vertex_data.reserve((sizeof(p_wavefront.positions[0]) + sizeof(p_wavefront.normals[0]) + sizeof(p_wavefront.uvs[0])) * p_wavefront.positions.size() / sizeof(float));
    uint32_t vertex_count = 0;

    std::unordered_map<uint64_t, uint32_t> index_map;

    size_t object_index_buffer_size = 0;
    std::vector<uint32_t> object_indices;

    for (auto element : p_wavefront.objects) {
      WavefrontObject object = element.second;

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
          kmath::Vec3 position = p_wavefront.positions[position_index];
          kmath::Vec3 normal = p_wavefront.normals[normal_index];
          kmath::Vec2 uv = p_wavefront.uvs[uv_index];
          result.push_vertex(position, normal, uv);

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
      for (size_t i = 0; i < object_index_buffer_size; i += 3) {
        result.push_triangle(Triangle(
          object_indices[i + 0],
          object_indices[i + 1],
          object_indices[i + 2]
        ));
      }
    }

    result.gpu_upload();

    return result;
  }
}
