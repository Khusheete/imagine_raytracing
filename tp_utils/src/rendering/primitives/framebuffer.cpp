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


#include "framebuffer.hpp"
#include "tp_utils/src/debug.hpp"

#include "thirdparty/glad/include/glad/glad.h"


namespace tputils {
  
  void Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, buffer_id);
  }


  void Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }


  bool Framebuffer::is_complete() const {
    return glCheckNamedFramebufferStatus(buffer_id, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
  }


  const Texture2D &Framebuffer::get_texture_attachment(int p_index) {
    return texture_attachments[p_index];
  }


  void Framebuffer::set_size(uint32_t p_width, uint32_t p_height) {
    if (p_width == width && p_height == height) return;
    
    depth_stensil_attachment.set_size(p_width, p_height);
    for (Texture2D &texture : texture_attachments) {
      texture.resize(p_width, p_height);
    }

    width = p_width;
    height = p_height;
  }


  void Framebuffer::_init() {
    glGenFramebuffers(1, &buffer_id);

    bind();

    std::vector<uint32_t> attachment_indices(texture_attachments.size());
    for (size_t i = 0; i < texture_attachments.size(); i++) {
      attachment_indices[i] = GL_COLOR_ATTACHMENT0 + i;
      glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_indices[i], GL_TEXTURE_2D, texture_attachments[i].get_texture_id(), 0);
    }
    glDrawBuffers(attachment_indices.size(), attachment_indices.data());

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_stensil_attachment.buffer_id);

    unbind();
  }


  Framebuffer::Framebuffer(Framebuffer &&p_other) {
    if (buffer_id) {
      glDeleteFramebuffers(1, &buffer_id);
    }

    buffer_id = p_other.buffer_id;
    width = p_other.width;
    height = p_other.height;
    texture_attachments = std::move(p_other.texture_attachments);
    depth_stensil_attachment = std::move(p_other.depth_stensil_attachment);
    p_other.buffer_id = 0;
  }
  
  Framebuffer &Framebuffer::operator=(Framebuffer &&p_other) {
    if (buffer_id) {
      glDeleteFramebuffers(1, &buffer_id);
    }

    buffer_id = p_other.buffer_id;
    width = p_other.width;
    height = p_other.height;
    texture_attachments = std::move(p_other.texture_attachments);
    depth_stensil_attachment = std::move(p_other.depth_stensil_attachment);
    p_other.buffer_id = 0;

    return *this;
  }


  Framebuffer::~Framebuffer() {
    if (buffer_id) glDeleteFramebuffers(1, &buffer_id);
  }


  void RenderBufferObject::bind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, buffer_id);
  }


  void RenderBufferObject::unbind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }


  void RenderBufferObject::set_size(int32_t p_width, int32_t p_height) {
    bind();
    glRenderbufferStorage(GL_RENDERBUFFER, get_internal_format(), p_width, p_height);
    unbind();
  }


  RenderBufferObject::RenderBufferObject(uint32_t p_width, uint32_t p_height, RenderBufferFormat p_format)
    : format(p_format)
  {
    glGenRenderbuffers(1, &buffer_id);
    set_size(p_width, p_height);
  }


  RenderBufferObject::RenderBufferObject(RenderBufferObject &&p_other) {
    if (buffer_id) {
      glDeleteRenderbuffers(1, &buffer_id);
    }

    buffer_id = p_other.buffer_id;
    format = p_other.format;
    p_other.buffer_id = 0;
  }
  
  RenderBufferObject &RenderBufferObject::operator=(RenderBufferObject &&p_other) {
    if (buffer_id) {
      glDeleteRenderbuffers(1, &buffer_id);
    }

    buffer_id = p_other.buffer_id;
    format = p_other.format;
    p_other.buffer_id = 0;

    return *this;
  }


  RenderBufferObject::~RenderBufferObject() {
    if (buffer_id) glDeleteRenderbuffers(1, &buffer_id);
  }


  uint32_t RenderBufferObject::get_internal_format() const {
    switch (format) {
      case RenderBufferFormat::RED:
        return GL_RED;
      case RenderBufferFormat::RG:
        return GL_RG;
      case RenderBufferFormat::RGB:
        return GL_RGB;
      case RenderBufferFormat::RGBA:
        return GL_RGBA;
      case RenderBufferFormat::DEPTH_STENSIL:
        return GL_DEPTH_STENCIL;
      default:
        ASSERT_FATAL_ERROR(false, "Unknown RenderBufferFormat");
        return 0;
    }
  }
}
