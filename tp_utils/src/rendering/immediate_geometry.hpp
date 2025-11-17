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


#include "thirdparty/kmath/vector.hpp"
#include "thirdparty/kmath/matrix.hpp"
#include "primitives/vbuffer.hpp"

#include <vector>


namespace tputils {
  class ImmediateGeometry {
  public:
    enum class Mode {
      POINTS,

      LINE_STRIP,
      LINE_LOOP,
      LINES,
      LINE_STRIP_ADJACENCY,
      
      TRIANGLE_STRIP,
      TRIANGLE_FAN,
      TRIANGLES,
      TRIANGLE_STRIP_ADJACENCY,
      TRIANGLES_ADJACENCY,
    };

  public:

    void begin(const Mode p_mode, const BufferLayout *p_layout);

    void push_float(const float p_float);
    void push_vec2(const kmath::Vec2 &p_vec2);
    void push_vec3(const kmath::Vec3 &p_vec3);
    void push_vec4(const kmath::Vec4 &p_vec4);
    void push_mat2(const kmath::Mat2 &p_mat2);
    void push_mat3(const kmath::Mat3 &p_mat3);
    void push_mat4(const kmath::Mat4 &p_mat4);
    void push_int(const int p_int);
    void push_ivec2(const kmath::Vec2i &p_ivec2);
    void push_ivec3(const kmath::Vec3i &p_ivec3);
    void push_ivec4(const kmath::Vec4i &p_ivec4);
    void push_bool(const bool p_bool);

    void end();

    ImmediateGeometry() = default;

  private:
    template<typename T>
    inline void _push_value(const T &p_value) {
      const uint8_t *data = reinterpret_cast<const uint8_t*>(&p_value);

      if (drawing_data_size == drawing_data.size()) {
        drawing_data.insert(drawing_data.end(), data, data + sizeof(T));
      } else {
        const long remaining_place = drawing_data.size() - drawing_data_size;
        const long writable = std::min(remaining_place, (long)sizeof(T));
        const long not_writable = sizeof(T) - writable;

        for (long i = 0; i < writable; i++) {
          drawing_data[drawing_data_size + i] = data[i];
        }

        if (not_writable > 0) [[unlikely]] {
          drawing_data.insert(drawing_data.end(), data + (size_t)writable, data + sizeof(T));
        }

      }
      drawing_data_size += sizeof(T);
    }


    inline void _next_attribute() {
      ++current_attribute;
      if (current_attribute == current_layout->end()) {
        current_attribute = current_layout->begin();
        vertex_count += 1;
      }
    }

    static GLenum _get_gl_mode(const Mode p_mode);
    
  private:
    std::vector<uint8_t> drawing_data;
    VertexBufferObject vbo;
    VBuffer vbuffer = VBuffer(VBuffer::Kind::VERTEX_BUFFER, VBuffer::Usage::STREAM_DRAW);
    size_t drawing_data_size = 0;
    long vertex_count = 0;

    Mode current_mode;
    const BufferLayout *current_layout;
    BufferLayout::Iterator current_attribute;
    bool is_drawing = false;
  };


  template<>
  inline void ImmediateGeometry::_push_value(const bool &p_value) {
    if (drawing_data_size == drawing_data.size()) {
      drawing_data.push_back(p_value);
    } else {
      drawing_data[drawing_data_size] = p_value;
    }
    drawing_data_size += 1;
  }
}
