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


#include "immediate_geometry.hpp"
#include "debug.hpp"
#include "rendering/primitives/shader.hpp"
#include "rendering/primitives/vbuffer.hpp"


namespace tputils {
  void ImmediateGeometry::begin(const Mode p_mode, const BufferLayout *p_layout) {
    ASSERT_WARNING(!is_drawing, "Immediate geometry is already drawing.");

    current_mode = p_mode;
    current_layout = p_layout;
    current_attribute = current_layout->begin();
    drawing_data_size = 0;
    vertex_count = 0;
    is_drawing = true;
  }


  void ImmediateGeometry::push_float(const float p_float) {
    ASSERT_WARNING((*current_attribute).data_type == ShaderDataType::Float, "Pushing the wrong type (float) to immediate geometry.");
    _push_value(p_float);
    _next_attribute();
  }


  void ImmediateGeometry::push_vec2(const kmath::Vec2 &p_vec2) {
    ASSERT_WARNING((*current_attribute).data_type == ShaderDataType::Vec2, "Pushing the wrong type (vec2) to immediate geometry.");
    _push_value(p_vec2);
    _next_attribute();
  }


  void ImmediateGeometry::push_vec3(const kmath::Vec3 &p_vec3) {
    ASSERT_WARNING((*current_attribute).data_type == ShaderDataType::Vec3, "Pushing the wrong type (vec3) to immediate geometry.");
    _push_value(p_vec3);
    _next_attribute();
  }


  void ImmediateGeometry::push_vec4(const kmath::Vec4 &p_vec4) {
    ASSERT_WARNING((*current_attribute).data_type == ShaderDataType::Vec4, "Pushing the wrong type (vec4) to immediate geometry.");
    _push_value(p_vec4);
    _next_attribute();
  }


  void ImmediateGeometry::push_mat2(const kmath::Mat2 &p_mat2) {
    ASSERT_WARNING((*current_attribute).data_type == ShaderDataType::Mat2, "Pushing the wrong type (mat2) to immediate geometry.");
    _push_value(p_mat2);
    _next_attribute();
  }


  void ImmediateGeometry::push_mat3(const kmath::Mat3 &p_mat3) {
    ASSERT_WARNING((*current_attribute).data_type == ShaderDataType::Mat3, "Pushing the wrong type (mat3) to immediate geometry.");
    _push_value(p_mat3);
    _next_attribute();
  }


  void ImmediateGeometry::push_mat4(const kmath::Mat4 &p_mat4) {
    ASSERT_WARNING((*current_attribute).data_type == ShaderDataType::Mat4, "Pushing the wrong type (mat4) to immediate geometry.");
    _push_value(p_mat4);
    _next_attribute();
  }


  void ImmediateGeometry::push_int(const int p_int) {
    ASSERT_WARNING((*current_attribute).data_type == ShaderDataType::Int, "Pushing the wrong type (int) to immediate geometry.");
    _push_value(p_int);
    _next_attribute();
  }


  void ImmediateGeometry::push_ivec2(const kmath::Vec2i &p_ivec2) {
    ASSERT_WARNING((*current_attribute).data_type == ShaderDataType::IVec2, "Pushing the wrong type (ivec2) to immediate geometry.");
    _push_value(p_ivec2);
    _next_attribute();
  }


  void ImmediateGeometry::push_ivec3(const kmath::Vec3i &p_ivec3) {
    ASSERT_WARNING((*current_attribute).data_type == ShaderDataType::IVec3, "Pushing the wrong type (ivec3) to immediate geometry.");
    _push_value(p_ivec3);
    _next_attribute();
  }


  void ImmediateGeometry::push_ivec4(const kmath::Vec4i &p_ivec4) {
    ASSERT_WARNING((*current_attribute).data_type == ShaderDataType::IVec4, "Pushing the wrong type (ivec4) to immediate geometry.");
    _push_value(p_ivec4);
    _next_attribute();
  }


  void ImmediateGeometry::push_bool(const bool p_bool) {
    ASSERT_WARNING((*current_attribute).data_type == ShaderDataType::Bool, "Pushing the wrong type (bool) to immediate geometry.");
    _push_value(p_bool);
    _next_attribute();
  }


  void ImmediateGeometry::end() {
    ASSERT_WARNING(current_attribute == current_layout->begin(), "Drawing immediate geometry but in the middle of specifying data for a vertex.");

    // Initialize vbo
    if (!vbo.is_initialized()) {
      vbo.initialize();
    }
    
    // Write data
    vbuffer.write_data<uint8_t>(std::span(drawing_data.begin(), drawing_data.begin() + drawing_data_size));

    // Draw
    vbo.bind();
    vbuffer.bind();

    // Setup attributes
    int index = 0;
    for (const BufferLayout::Attribute &attrib : *current_layout) {
      glVertexAttribPointer(
        index,
        attrib.component_count,
        attrib.gl_type,
        GL_FALSE,
        current_layout->get_stride(),
        reinterpret_cast<void*>(attrib.offset)
      );
      glEnableVertexAttribArray(index);
      index += 1;
    }

    glDrawArrays(_get_gl_mode(current_mode), 0, vertex_count);

    vbuffer.unbind();
    vbo.unbind();

    is_drawing = false;
  }


  GLenum ImmediateGeometry::_get_gl_mode(const Mode p_mode) {
    switch (p_mode) {
    case Mode::POINTS:                   return GL_POINTS;
    case Mode::LINE_STRIP:               return GL_LINE_STRIP;
    case Mode::LINE_LOOP:                return GL_LINE_LOOP;
    case Mode::LINES:                    return GL_LINES;
    case Mode::LINE_STRIP_ADJACENCY:     return GL_LINE_STRIP_ADJACENCY;
    case Mode::TRIANGLE_STRIP:           return GL_TRIANGLE_STRIP;
    case Mode::TRIANGLE_FAN:             return GL_TRIANGLE_FAN;
    case Mode::TRIANGLES:                return GL_TRIANGLES;
    case Mode::TRIANGLE_STRIP_ADJACENCY: return GL_TRIANGLE_STRIP_ADJACENCY;
    case Mode::TRIANGLES_ADJACENCY:      return GL_TRIANGLES_ADJACENCY;
    }
    return 0;
  }
}
