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


#include "texture.hpp"

#include "debug.hpp"

#include "thirdparty/stb/stb_image.h"

#include "thirdparty/glad/include/glad/glad.h"
#include <filesystem>


namespace tputils {


    void Texture2D::bind(const uint32_t p_location) const {
        glActiveTexture(GL_TEXTURE0 + p_location);
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }


    void Texture2D::unbind(const uint32_t p_location) const {
        glActiveTexture(GL_TEXTURE0 + p_location);
        glBindTexture(GL_TEXTURE_2D, 0);
    }


    bool Texture2D::is_valid() const {
        return valid;
    }


    uint32_t Texture2D::get_texture_id() const {
        return texture_id;
    }


    void Texture2D::resize(uint32_t p_width, uint32_t p_height) {
        uint32_t internal_format = 0, gl_format = 0;
        Texture2D::get_opengl_format(format, &internal_format, &gl_format);

        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, p_width, p_height, 0, gl_format, GL_UNSIGNED_BYTE, nullptr);
        unbind();

        width = p_width;
        height = p_height;
    }


    void Texture2D::generate_mipmaps() {
      glGenerateTextureMipmap(texture_id);
    }


    Texture2D Texture2D::load(const std::filesystem::path &p_path) {
        int width = 0, height = 0, channel_count = 0;
        stbi_set_flip_vertically_on_load(1);
// #ifdef WINDOWS // FIXME: adapt for windows build
//         unsigned char *image_data = stbi_load(p_path.string().c_str(), &width, &height, &channel_count, 0);
// #else
        unsigned char *image_data = stbi_load(p_path.c_str(), &width, &height, &channel_count, 0);
// #endif
        
        if (!image_data) {
            LOG_ERROR("Could not laod image at " << p_path);
            return Texture2D(); // Return invalid image
        }

        TextureFormat format;
        switch (channel_count) {
        case 1:
            format = TextureFormat::R8;
            break;
        case 2:
            format = TextureFormat::RG8;
            break;
        case 3:
            format = TextureFormat::RGB8;
            break;
        case 4:
            format = TextureFormat::RGBA8;
            break;
        default:
            ASSERT_FATAL_ERROR(false, "Invalid channel count (this should never happen)")
        }

        Texture2D texture(image_data, width, height, format);
        stbi_image_free(image_data);
        return texture;
    }


    Texture2D::Texture2D(int p_width, int p_height, TextureFormat p_format)
        : format(p_format),
        width(p_width),
        height(p_height)
    {
        glGenTextures(1, &texture_id);

        uint32_t internal_format = 0, gl_format = 0;
        Texture2D::get_opengl_format(p_format, &internal_format, &gl_format);

        bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, p_width, p_height, 0, gl_format, GL_UNSIGNED_BYTE, nullptr);

        unbind();
        valid = true;
    }


    Texture2D::Texture2D(const void *p_data, int p_width, int p_height, TextureFormat p_format)
        : format(p_format),
        width(p_width),
        height(p_height)
    {
        glGenTextures(1, &texture_id);

        uint32_t internal_format = 0, gl_format = 0;
        Texture2D::get_opengl_format(p_format, &internal_format, &gl_format);

        bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, internal_format, p_width, p_height, 0, gl_format, GL_UNSIGNED_BYTE, p_data);

        unbind();
        valid = true;
    }


    Texture2D::Texture2D(Texture2D &&p_other)
        : texture_id(p_other.texture_id),
        valid(p_other.valid),
        format(p_other.format),
        width(p_other.width),
        height(p_other.height)
    {
        p_other.texture_id = 0;
        p_other.valid = false;
    }
    
    
    Texture2D &Texture2D::operator=(Texture2D &&p_other) {
        if (valid) {
            glDeleteTextures(1, &texture_id);
        }

        texture_id = p_other.texture_id;
        valid = p_other.valid;
        format = p_other.format;
        width = p_other.width;
        height = p_other.height;

        p_other.texture_id = 0;
        p_other.valid = 0;

        return *this;
    }

    void Texture2D::get_opengl_format(TextureFormat p_tex_format, uint32_t *p_internal_format, uint32_t *p_format) {
        switch (p_tex_format) {
        case TextureFormat::R8:
            *p_internal_format = GL_R8;
            *p_format = GL_RED;
            break;
        case TextureFormat::RG8:
            *p_internal_format = GL_RG8;
            *p_format = GL_RG;
            break;
        case TextureFormat::RGB8: // TODO: check conversion
            *p_internal_format = GL_RGB8;
            *p_format = GL_RGB;
            break;
        case TextureFormat::RGBA8:
            *p_internal_format = GL_RGBA8;
            *p_format = GL_RGBA;
            break;
        case TextureFormat::R16:
            *p_internal_format = GL_R16;
            *p_format = GL_RED;
            break;
        case TextureFormat::RG16:
            *p_internal_format = GL_RG16;
            *p_format = GL_RG;
            break;
        case TextureFormat::RGB16: // TODO: check conversion
            *p_internal_format = GL_RGB16;
            *p_format = GL_RGB;
            break;
        case TextureFormat::RGBA16:
            *p_internal_format = GL_RGBA16;
            *p_format = GL_RGBA;
            break;
        case TextureFormat::R16F:
            *p_internal_format = GL_R16F;
            *p_format = GL_RED;
            break;
        case TextureFormat::RG16F:
            *p_internal_format = GL_RG16F;
            *p_format = GL_RG;
            break;
        case TextureFormat::RGB16F:
            *p_internal_format = GL_RGB16F;
            *p_format = GL_RGB;
            break;
        case TextureFormat::RGBA16F:
            *p_internal_format = GL_RGBA16F;
            *p_format = GL_RGBA;
            break;
        case TextureFormat::R32F:
            *p_internal_format = GL_R32F;
            *p_format = GL_RED;
            break;
        case TextureFormat::RG32F:
            *p_internal_format = GL_RG32F;
            *p_format = GL_RG;
            break;
        case TextureFormat::RGB32F:
            *p_internal_format = GL_RGB32F;
            *p_format = GL_RGB;
            break;
        case TextureFormat::RGBA32F:
            *p_internal_format = GL_RGBA32F;
            *p_format = GL_RGBA;
            break;
        }
    }
    
    
    Texture2D::~Texture2D() {
        if (valid) glDeleteTextures(1, &texture_id);
    }


    static GLenum get_min_filter_gl_enum(const Sampler::MinFilter p_filter) {
      switch (p_filter) {
      case Sampler::MinFilter::NEAREST:                return GL_NEAREST;
      case Sampler::MinFilter::LINEAR:                 return GL_LINEAR;
      case Sampler::MinFilter::NEAREST_MIPMAP_NEAREST: return GL_NEAREST_MIPMAP_NEAREST;
      case Sampler::MinFilter::NEAREST_MIPMAP_LINEAR:  return GL_NEAREST_MIPMAP_LINEAR;
      case Sampler::MinFilter::LINEAR_MIPMAP_NEAREST:  return GL_LINEAR_MIPMAP_NEAREST;
      case Sampler::MinFilter::LINEAR_MIPMAP_LINEAR:   return GL_LINEAR_MIPMAP_LINEAR;
      }
      LOG_ERROR("Should be dead code");
      return GL_LINEAR;
    }


    static GLenum get_mag_filter_gl_enum(const Sampler::MagFilter p_filter) {
      switch (p_filter) {
      case Sampler::MagFilter::NEAREST: return GL_NEAREST;
      case Sampler::MagFilter::LINEAR:  return GL_LINEAR;
      }
      LOG_ERROR("Should be dead code");
      return GL_LINEAR;
    }


    static GLenum get_wrap_mode_gl_enum(const Sampler::WrapMode p_wrap_mode) {
      switch (p_wrap_mode) {
      case Sampler::WrapMode::CLAMP_TO_EDGE:          return GL_CLAMP_TO_EDGE;
      case Sampler::WrapMode::REPEAT:                 return GL_REPEAT;
      case Sampler::WrapMode::MIRRORED_CLAMP_TO_EDGE: return GL_MIRROR_CLAMP_TO_EDGE;
      case Sampler::WrapMode::MIRRORED_REPEAT:        return GL_MIRRORED_REPEAT;
      case Sampler::WrapMode::CLAMP_TO_BORDER:        return GL_CLAMP_TO_BORDER;
      }
      LOG_ERROR("Should be dead code");
      return GL_REPEAT;
    }


    void Sampler::bind(const uint32_t p_location) const {
      glBindSampler(GL_TEXTURE0 + p_location, sampler_id);
    }


    void Sampler::unbind(const uint32_t p_location) const {
      glBindSampler(GL_TEXTURE0 + p_location, 0);
    }


    void Sampler::set_config(const Sampler2DConfig &p_config) {
      if (!sampler_id) glGenSamplers(1, &sampler_id);

      glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, get_min_filter_gl_enum(p_config.minification_filter));
      glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, get_mag_filter_gl_enum(p_config.magnification_filer));

      glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, get_wrap_mode_gl_enum(p_config.x_wrap_mode));
      glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, get_wrap_mode_gl_enum(p_config.y_wrap_mode));

      glSamplerParameterfv(sampler_id, GL_TEXTURE_BORDER_COLOR, reinterpret_cast<const float*>(&p_config.border_color));

      glSamplerParameterf(sampler_id, GL_TEXTURE_MIN_LOD, p_config.minimum_lod);
      glSamplerParameterf(sampler_id, GL_TEXTURE_MAX_LOD, p_config.maximum_lod);
    }


    void Sampler::set_config(const Sampler3DConfig &p_config) {
      if (!sampler_id) glGenSamplers(1, &sampler_id);

      glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, get_min_filter_gl_enum(p_config.minification_filter));
      glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, get_mag_filter_gl_enum(p_config.magnification_filer));

      glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, get_wrap_mode_gl_enum(p_config.x_wrap_mode));
      glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, get_wrap_mode_gl_enum(p_config.y_wrap_mode));
      glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_R, get_wrap_mode_gl_enum(p_config.z_wrap_mode));

      glSamplerParameterfv(sampler_id, GL_TEXTURE_BORDER_COLOR, reinterpret_cast<const float*>(&p_config.border_color));

      glSamplerParameterf(sampler_id, GL_TEXTURE_MIN_LOD, p_config.minimum_lod);
      glSamplerParameterf(sampler_id, GL_TEXTURE_MAX_LOD, p_config.maximum_lod);
    }


    Sampler Sampler::from_config(const Sampler2DConfig &p_config) {
      Sampler result;
      result.set_config(p_config);
      return result;
    }


    Sampler Sampler::from_config(const Sampler3DConfig &p_config) {
      Sampler result;
      result.set_config(p_config);
      return result;
    }


    Sampler::Sampler(Sampler &&p_other) {
      if (sampler_id) glDeleteSamplers(1, &sampler_id);

      sampler_id = p_other.sampler_id;
      p_other.sampler_id = 0;
    }


    Sampler &Sampler::operator=(Sampler &&p_other) {
      if (sampler_id) glDeleteSamplers(1, &sampler_id);

      sampler_id = p_other.sampler_id;
      p_other.sampler_id = 0;

      return *this;
    }


    Sampler::~Sampler() {
      if (sampler_id) glDeleteSamplers(1, &sampler_id);
    }
}
