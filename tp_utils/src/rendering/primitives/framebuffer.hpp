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


#include "texture.hpp"

#include <cstdint>
#include <ranges>
#include <vector>


namespace tputils {

  class Framebuffer;


  class RenderBufferObject {
    public:
      enum class RenderBufferFormat {
        RED, RG, RGB, RGBA, DEPTH_STENSIL,
      };

    public:
      void bind() const;
      void unbind() const;

      void set_size(int32_t p_width, int32_t p_height);

      RenderBufferObject() = default;
      RenderBufferObject(uint32_t p_width, uint32_t p_height, RenderBufferFormat p_format);
      RenderBufferObject(const RenderBufferObject &p_other) = delete;
      RenderBufferObject(RenderBufferObject &&p_other);
      RenderBufferObject &operator=(RenderBufferObject &&p_other);
      ~RenderBufferObject();
    
    private:
      uint32_t get_internal_format() const;

    private:
      uint32_t buffer_id = 0;
      RenderBufferFormat format;

      friend Framebuffer;
  };


  class Framebuffer {
    public:
      void bind() const;
      void unbind() const;

      bool is_complete() const;

      inline uint32_t get_width() const { return width; }
      inline uint32_t get_height() const { return height; }

      inline size_t get_attachment_count() { return texture_attachments.size(); }
      const Texture2D &get_texture_attachment(int p_index);

      void set_size(uint32_t p_width, uint32_t p_height);

      Framebuffer() = default;

      template<typename Iterable>
      requires std::ranges::sized_range<Iterable>
      Framebuffer(uint32_t p_width, uint32_t p_height, const Iterable &p_texture_formats)
        : width(p_width),
        height(p_height),
        depth_stensil_attachment(p_width, p_height, RenderBufferObject::RenderBufferFormat::DEPTH_STENSIL)
      {
        const size_t tex_count = p_texture_formats.size();
        texture_attachments.reserve(tex_count);
        for (const TextureFormat &format : p_texture_formats) {
          texture_attachments.emplace_back(p_width, p_height, format);
        }

        _init();
      }

      Framebuffer(const Framebuffer &p_other) = delete;
      Framebuffer(Framebuffer &&p_other);
      Framebuffer &operator=(Framebuffer &&p_other);
      ~Framebuffer();

    private:
      void _init();

    private:
      uint32_t buffer_id = 0;
      uint32_t width, height;

      std::vector<Texture2D> texture_attachments;
      RenderBufferObject depth_stensil_attachment;
  };
}
