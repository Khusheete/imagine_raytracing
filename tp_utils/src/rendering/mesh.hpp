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


#include "../model_loaders/off_object.hpp"
#include "../model_loaders/wavefront_object.hpp"
#include "primitives/vbuffer.hpp"
#include "thirdparty/kmath/vector.hpp"

#include <cstdint>
#include <vector>


namespace tputils {

  typedef kmath::_Vec3<uint32_t> Triangle;

  class TriangleMesh {
  public:
    enum class VertexKind : unsigned char {
      POSITION           = 0b00,
      POSITION_NORMAL    = 0b01,
      POSITION_UV        = 0b10,
      POSITION_NORMAL_UV = POSITION_NORMAL | POSITION_UV,
    };

  public:
    inline bool has_uv() const { return (static_cast<int>(vertex_kind) & static_cast<int>(VertexKind::POSITION_UV)); };
    inline bool has_normal() const { return (static_cast<int>(vertex_kind) & static_cast<int>(VertexKind::POSITION_NORMAL)); };
    inline size_t get_vertex_size() const {
      switch (vertex_kind) {
      case VertexKind::POSITION:           return 3;
      case VertexKind::POSITION_NORMAL:    return 6;
      case VertexKind::POSITION_UV:        return 5;
      case VertexKind::POSITION_NORMAL_UV: return 8;
      }
      return 0;
    }

    inline const kmath::Vec3 &get_vertex_position(const size_t p_index) const {
      return *reinterpret_cast<const kmath::Vec3*>(&vertex_data[get_vertex_size() * p_index]);
    }
    inline const kmath::Vec3 &get_vertex_normal(const size_t p_index) const {
      return *reinterpret_cast<const kmath::Vec3*>(&vertex_data[get_vertex_size() * p_index + 3]);
    }
    inline const kmath::Vec2 &get_vertex_uv(const size_t p_index) const {
      return *reinterpret_cast<const kmath::Vec2*>(&vertex_data[get_vertex_size() * p_index + 3 + ((has_normal())? 3 : 0)]);
    }
    inline kmath::Vec3 &get_vertex_position(const size_t p_index) { return const_cast<kmath::Vec3&>((const_cast<const TriangleMesh*>(this))->get_vertex_position(p_index)); }
    inline kmath::Vec3 &get_vertex_normal(const size_t p_index) { return const_cast<kmath::Vec3&>((const_cast<const TriangleMesh*>(this))->get_vertex_normal(p_index)); }
    inline kmath::Vec2 &get_vertex_uv(const size_t p_index) { return const_cast<kmath::Vec2&>((const_cast<const TriangleMesh*>(this))->get_vertex_uv(p_index)); }

    inline size_t get_vertex_count() const { return vertex_data.size() / get_vertex_size(); }

    inline const Triangle &get_triangle(const size_t p_index) const {
      return triangles[p_index];
    }
    inline Triangle &get_triangle(const size_t p_index) {
      return triangles[p_index];
    }

    inline size_t get_triangle_count() const { return triangles.size(); }

    void reserve_vertices(const size_t p_size);
    void resize_vertices(const size_t p_size);
    void push_vertex(const kmath::Vec3 &p_position);
    void push_vertex(const kmath::Vec3 &p_position, const kmath::Vec3 &p_normal);
    void push_vertex(const kmath::Vec3 &p_position, const kmath::Vec2 &p_uv);
    void push_vertex(const kmath::Vec3 &p_position, const kmath::Vec3 &p_normal, const kmath::Vec2 &p_uv);

    void reserve_triangles(const size_t p_size);
    void resize_triangles(const size_t p_size);
    void push_triangle(const Triangle &p_triangle);

    void gpu_upload();
    void draw();
    void draw_wireframe();

    TriangleMesh(const VertexKind p_vertex_kind = VertexKind::POSITION);
    static TriangleMesh from_off_object(const OffObject &p_off);
    static TriangleMesh from_wavefront_mesh(const WavefrontMesh &p_obj);

  private:
    std::vector<float> vertex_data;
    std::vector<Triangle> triangles;

    VBuffer vertex_buffer = VBuffer(VBuffer::Kind::VERTEX_BUFFER, VBuffer::Usage::STATIC_DRAW);
    VBuffer index_buffer = VBuffer(VBuffer::Kind::INDEX_BUFFER, VBuffer::Usage::STATIC_DRAW);
    VertexBufferObject vab;

    VertexKind vertex_kind;
  };
}
