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

#include "../../debug.hpp"
#include "thirdparty/glad/include/glad/glad.h"

#include <cstdint>
#include <span>


namespace tputils {

  enum class ShaderDataType;

  class BufferLayout {
  public:
    struct Attribute {
      uint32_t offset;
      uint32_t component_count;
      GLint gl_type;
      ShaderDataType data_type;
    };

    struct Iterator {
    public:
      Iterator &operator++();
      Attribute operator*() const;
      inline bool operator==(const Iterator &p_other) { return attribute_iterator == p_other.attribute_iterator; }
      inline bool operator!=(const Iterator &p_other) { return attribute_iterator != p_other.attribute_iterator; }

      inline Iterator(std::span<const ShaderDataType>::iterator p_attribute_iterator)
      : attribute_iterator(p_attribute_iterator), current_offset(0) {}
      inline Iterator(): attribute_iterator(), current_offset(0) {}
      
    private:
      std::span<const ShaderDataType>::iterator attribute_iterator;
      uint32_t current_offset;
    };
    
  public:
    inline uint32_t get_stride() const { return stride; }
    inline uint32_t get_layout_length() const { return attributes.size(); }

    inline Iterator begin() const { return Iterator(attributes.begin()); }
    inline Iterator end() const { return Iterator(attributes.end()); }

    BufferLayout(const std::span<const ShaderDataType> &p_layout);

  private:
    const std::span<const ShaderDataType> attributes;
    uint32_t stride;
  };


  class VBuffer {
  public:
    enum class Kind {
      VERTEX_BUFFER,
      INDEX_BUFFER,
      STORAGE_BUFFER,
      UNIFORM_BUFFER,
      TEXTURE_BUFFER,
    };

    enum class Usage {
      STATIC_DRAW,
      STATIC_GPU_TO_CPU,
      STATIC_GPU_TO_GPU,
      STREAM_DRAW,
      STREAM_GPU_TO_CPU,
      STREAM_GPU_TO_GPU,
      DYNAMIC_DRAW,
      DYNAMIC_GPU_TO_CPU,
      DYNAMIC_GPU_TO_GPU,
    };

    class Slice {
    public:
      void bind_to_location(uint32_t p_bind_location) const;

      Slice() = default;
    private:
      inline Slice(uint32_t p_buffer_id, uint32_t p_offset, uint32_t p_size)
        : buffer_id(p_buffer_id),
        offset(p_offset),
        size(p_size) {}

    private:
      uint32_t buffer_id = 0;
      uint32_t offset;
      uint32_t size;
      
      friend VBuffer;
    };

  public:
    void bind() const;
    void unbind() const;

    void allocate(const uint32_t p_size);
    Slice data() const;
    Slice slice(uint32_t p_offset, uint32_t p_size) const;

    template<typename T>
    void write_data(const std::span<const T> p_data) {
      if (!buffer_id) glGenBuffers(1, &buffer_id);
      
      size = p_data.size() * sizeof(T);
      bind();
      glBufferData(_get_gl_buffer_kind(kind), size, p_data.data(), _get_gl_usage(usage));
      unbind();
    }

    template<typename T>
    void write_subdata(const std::span<const T> p_data, const uint32_t p_offset = 0) {
      const uint32_t data_size = p_data.size() * sizeof(T);
      ASSERT_WARNING(p_offset + data_size < size, "VBuffer write out of bound: " << p_offset << " + " << data_size << " >= " << size);

      bind();
      glBufferSubData(_get_gl_buffer_kind(kind), p_offset, data_size, p_data.data());
      unbind();
    }

    inline uint32_t get_size() const { return size; }
    inline Kind get_kind() const { return kind; }
    inline Usage get_usage() const { return usage; }

    VBuffer(const Kind p_kind, const Usage p_usage);

    VBuffer(const VBuffer&) = delete;
    VBuffer &operator=(const VBuffer&) = delete;

    VBuffer(VBuffer &&p_other);
    VBuffer &operator=(VBuffer &&p_other);

    ~VBuffer();

  private:
    inline static GLenum _get_gl_buffer_kind(const Kind p_kind) {
      switch (p_kind) {
      case Kind::VERTEX_BUFFER:  return GL_ARRAY_BUFFER;
      case Kind::INDEX_BUFFER:   return GL_ELEMENT_ARRAY_BUFFER;
      case Kind::STORAGE_BUFFER: return GL_SHADER_STORAGE_BUFFER;
      case Kind::UNIFORM_BUFFER: return GL_UNIFORM_BUFFER;
      case Kind::TEXTURE_BUFFER: return GL_TEXTURE_BUFFER;
      }
      return 0; // Dead code
    }
    
    inline static GLenum _get_gl_usage(const Usage p_usage) {
      switch (p_usage) {
      case Usage::STATIC_DRAW:        return GL_STATIC_DRAW;
      case Usage::STATIC_GPU_TO_CPU:  return GL_STATIC_READ;
      case Usage::STATIC_GPU_TO_GPU:  return GL_STATIC_COPY;
      case Usage::STREAM_DRAW:        return GL_STREAM_DRAW;
      case Usage::STREAM_GPU_TO_CPU:  return GL_STREAM_READ;
      case Usage::STREAM_GPU_TO_GPU:  return GL_STREAM_COPY;
      case Usage::DYNAMIC_DRAW:       return GL_DYNAMIC_DRAW;
      case Usage::DYNAMIC_GPU_TO_CPU: return GL_DYNAMIC_READ;
      case Usage::DYNAMIC_GPU_TO_GPU: return GL_DYNAMIC_COPY;
      }
      return 0; // Dead code
    }

  private:
    uint32_t buffer_id = 0;
    uint32_t size;
    Kind kind;
    Usage usage;
  };


  class VertexBufferObject {
  public:
    void bind() const;
    void unbind() const;

    void initialize();
    void add_vertex_buffer(const VBuffer &p_vertex_buffer, const BufferLayout &p_buffer_layout);
    void add_index_buffer(const VBuffer &p_index_buffer);

    inline bool is_initialized() const { return array_index; }

    VertexBufferObject();
    VertexBufferObject(const VertexBufferObject&) = delete;
    VertexBufferObject(VertexBufferObject &&p_other);
    VertexBufferObject &operator=(VertexBufferObject &&p_other);
    ~VertexBufferObject();

  private:
    uint32_t array_index = 0;
    uint32_t vertex_attrib_count = 0;
  };
}
