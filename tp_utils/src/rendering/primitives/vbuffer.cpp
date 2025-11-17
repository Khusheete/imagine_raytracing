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


#include "vbuffer.hpp"
#include "debug.hpp"
#include "shader.hpp"

#include "thirdparty/glad/include/glad/glad.h"

#include <cstdint>


namespace tputils {

  BufferLayout::Iterator &BufferLayout::Iterator::operator++() {
    current_offset += shader_data_type_get_size(*attribute_iterator);
    ++attribute_iterator;
    return *this;
  }


  BufferLayout::Attribute BufferLayout::Iterator::operator*() const {
    const ShaderDataType attribute = *attribute_iterator;
    return Attribute(
      current_offset,
      shader_data_type_get_component_count(attribute),
      shader_data_type_get_gl_type(attribute),
      attribute
    );
  }


  BufferLayout::BufferLayout(const std::span<const ShaderDataType> &p_layout)
    : attributes(p_layout)
  {
    stride = 0;
    for (uint32_t i = 0; i < attributes.size(); i++) {
      stride += shader_data_type_get_size(p_layout[i]);
    }
  }


  void VBuffer::bind() const {
    glBindBuffer(VBuffer::_get_gl_buffer_kind(kind), buffer_id);
  }


  void VBuffer::unbind() const {
    glBindBuffer(VBuffer::_get_gl_buffer_kind(kind), 0);
  }


  void VBuffer::allocate(const uint32_t p_size) {
    size = p_size;
    bind();
    glBufferData(_get_gl_buffer_kind(kind), size, nullptr, _get_gl_usage(usage));
    unbind();
  }


  VBuffer::Slice VBuffer::data() const {
    return Slice(buffer_id, 0, size);
  }


  VBuffer::Slice VBuffer::slice(uint32_t p_offset, uint32_t p_size) const {
    ASSERT_WARNING(kind == Kind::STORAGE_BUFFER || kind == Kind::UNIFORM_BUFFER, "Cannot slice a VBuffer other than a storage and uniform buffer");
    ASSERT_WARNING(p_offset + p_size < p_size, "Slicing buffer out of range: " << p_offset << " + " << p_size << " >= " << size);
    return Slice(buffer_id, p_offset, p_size);
  }


  void VBuffer::Slice::bind_to_location(uint32_t p_bind_location) const {
    if (buffer_id) {
      glBindBufferRange(GL_SHADER_STORAGE_BUFFER, p_bind_location, buffer_id, offset, size);
    }
  }


  VBuffer::VBuffer(const Kind p_kind, const Usage p_usage) : kind(p_kind), usage(p_usage) {}


  VBuffer::VBuffer(VBuffer &&p_other): buffer_id(p_other.buffer_id) {
    p_other.buffer_id = 0;
  }


  VBuffer &VBuffer::operator=(VBuffer &&p_other) {
    if (buffer_id) glDeleteBuffers(1, &buffer_id);
    
    buffer_id = p_other.buffer_id;

    p_other.buffer_id = 0;

    return *this;
  }


  VBuffer::~VBuffer() {
    if (buffer_id) {
      glDeleteBuffers(1, &buffer_id);
    }
  }


  void VertexBufferObject::bind() const {
    glBindVertexArray(array_index);
  }


  void VertexBufferObject::unbind() const {
    glBindVertexArray(0);
  }


  void VertexBufferObject::initialize() {
    ASSERT_WARNING(array_index == 0, "Initializing already initialized VertexArrayBuffer, will leak memory");
    glGenVertexArrays(1, &array_index);
  }


  void VertexBufferObject::add_vertex_buffer(const VBuffer &p_vertex_buffer, const BufferLayout &p_buffer_layout) {
    ASSERT_WARNING(p_vertex_buffer.get_kind() == VBuffer::Kind::VERTEX_BUFFER, "Registering non vertex buffer as vertex buffer in VAB");
    
    bind();
    p_vertex_buffer.bind();
    
    for (BufferLayout::Attribute attrib : p_buffer_layout) {
      glVertexAttribPointer(
        vertex_attrib_count,
        attrib.component_count,
        attrib.gl_type,
        GL_FALSE,
        p_buffer_layout.get_stride(),
        reinterpret_cast<void*>(attrib.offset)
      );
      glEnableVertexAttribArray(vertex_attrib_count);
      vertex_attrib_count += 1;
    }
    unbind();
  }


  void VertexBufferObject::add_index_buffer(const VBuffer &p_index_buffer) {
    ASSERT_WARNING(p_index_buffer.get_kind() == VBuffer::Kind::INDEX_BUFFER, "Registering non element buffer as element buffer in VAB");

    bind();
    p_index_buffer.bind();
    unbind();
  }


  VertexBufferObject::VertexBufferObject(): vertex_attrib_count(0) {}


  VertexBufferObject::VertexBufferObject(VertexBufferObject &&p_other)
    : array_index(p_other.array_index),
    vertex_attrib_count(p_other.vertex_attrib_count)
  {
    p_other.array_index = 0;
    p_other.vertex_attrib_count = 0;
  }


  VertexBufferObject &VertexBufferObject::operator=(VertexBufferObject &&p_other) {
    if (array_index) glDeleteVertexArrays(1, &array_index);
    
    array_index = p_other.array_index;
    vertex_attrib_count = p_other.vertex_attrib_count;

    p_other.array_index = 0;
    p_other.vertex_attrib_count = 0;
    
    return *this;
  }


  VertexBufferObject::~VertexBufferObject() {
    if (array_index) {
      glDeleteVertexArrays(1, &array_index);
    }
  }
}
