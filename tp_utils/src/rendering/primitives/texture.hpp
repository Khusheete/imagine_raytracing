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


#include "thirdparty/kmath/color.hpp"
#include <cstdint>
#include <filesystem>


namespace tputils {

    class Framebuffer;


    enum class TextureFormat {
        R8, RG8, RGB8, RGBA8,
        R16, RG16, RGB16, RGBA16,
        R16F, RG16F, RGB16F, RGBA16F,
        R32F, RG32F, RGB32F, RGBA32F,
    };


    class BaseTexture {
    public:
        virtual void bind(const uint32_t p_location = 0) const = 0;
        virtual void unbind(const uint32_t p_location = 0) const = 0;
        virtual bool is_valid() const = 0;
        virtual uint32_t get_texture_id() const = 0;
    };


    class Texture2D : public BaseTexture {
    public:
        virtual void bind(const uint32_t p_location = 0) const override;
        virtual void unbind(const uint32_t p_location = 0) const override;

        virtual bool is_valid() const override;
        virtual uint32_t get_texture_id() const override;

        void resize(uint32_t p_width, uint32_t p_height);
        void generate_mipmaps();

        static Texture2D load(const std::filesystem::path &p_path);

        Texture2D() = default;
        Texture2D(int p_width, int p_height, TextureFormat p_format);
        Texture2D(const void *p_data, int p_width, int p_height, TextureFormat p_format);
        Texture2D(Texture2D &&p_other);
        Texture2D &operator=(Texture2D &&p_other);
        Texture2D(const Texture2D&) = delete;
        Texture2D &operator=(const Texture2D&) = delete;
        virtual ~Texture2D();

    private:
        void get_opengl_format(TextureFormat p_tex_format, uint32_t *p_internal_format, uint32_t *p_format);

    private:
        uint32_t texture_id = 0;
        bool valid = false;

        TextureFormat format;
        uint32_t width, height;

        friend Framebuffer;
    };


    class Sampler {
    public:
      enum class MinFilter {
        NEAREST,
        LINEAR,
        NEAREST_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_NEAREST,
        LINEAR_MIPMAP_LINEAR,
      };

      enum class MagFilter {
        NEAREST,
        LINEAR,
      };

      enum class WrapMode {
        CLAMP_TO_EDGE,
        REPEAT,
        MIRRORED_CLAMP_TO_EDGE,
        MIRRORED_REPEAT,
        CLAMP_TO_BORDER,
      };

      struct Sampler2DConfig {
        MinFilter minification_filter;
        MagFilter magnification_filer;
        WrapMode x_wrap_mode = WrapMode::CLAMP_TO_EDGE;
        WrapMode y_wrap_mode = WrapMode::CLAMP_TO_EDGE;
        kmath::Rgba border_color = kmath::Rgba::ZERO;
        float minimum_lod = -1000.0;
        float maximum_lod =  1000.0;
      };

      struct Sampler3DConfig {
        MinFilter minification_filter;
        MagFilter magnification_filer;
        WrapMode x_wrap_mode = WrapMode::CLAMP_TO_EDGE;
        WrapMode y_wrap_mode = WrapMode::CLAMP_TO_EDGE;
        WrapMode z_wrap_mode = WrapMode::CLAMP_TO_EDGE;
        kmath::Rgba border_color = kmath::Rgba::ZERO;
        float minimum_lod = -1000.0;
        float maximum_lod =  1000.0;
      };
    
    public:
      void bind(const uint32_t p_location = 0) const;
      void unbind(const uint32_t p_location = 0) const;

      inline bool is_valid() const { return sampler_id != 0; }

      void set_config(const Sampler2DConfig &p_config);
      void set_config(const Sampler3DConfig &p_config);

      static Sampler from_config(const Sampler2DConfig &p_config);
      static Sampler from_config(const Sampler3DConfig &p_config);

      Sampler() = default;
      Sampler(Sampler &&p_other);
      Sampler &operator=(Sampler &&p_other);
      Sampler(const Sampler&) = delete;
      Sampler &operator=(const Sampler&) = delete;
      ~Sampler();

    private:
      uint32_t sampler_id = 0;

      friend BaseTexture;
      friend Texture2D;
    };
}
