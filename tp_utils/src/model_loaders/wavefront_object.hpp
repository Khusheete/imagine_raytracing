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

#include <filesystem>
#include <map>
#include <string>
#include <vector>


namespace tputils {

  struct WavefrontObject;


  struct WavefrontMaterial {
    enum class IlluminationModel {
      COLOR_ON_AMBIENT_OFF             = 0,
      COLOR_ON_AMBIENT_ON              = 1,
      HIGHLIGH_ON                      = 2,
      RT_REFLECTION                    = 3,
      RT_TRANSPARENCY_REFLECTION       = 4,
      RT_FRESNEL_REFLECTION            = 5,
      RT_REFRACTION_REFLECTION         = 6,
      RT_REFRACTION_FRESNEL_REFLECTION = 7,
      REFLECTION                       = 8,
      TRANSPARENCY_REFLECTION          = 9,
      CAST_INVIS_SHADOW                = 10,
    };

    struct TextureMap {
      std::filesystem::path path = "";
      kmath::Vec3 origin_offset = kmath::Vec3(0.0, 0.0, 0.0);
      kmath::Vec3 scale = kmath::Vec3(1.0, 1.0, 1.0);
    };

    IlluminationModel illumination_model;

    kmath::Vec3 ambient_color;
    kmath::Vec3 diffuse_color;
    kmath::Vec3 specular_color;
    float specular_exponent;

    float dissolve;
    kmath::Vec3 transmission_filter_color;
    float index_of_refraction;

    TextureMap ambient_texture;
    TextureMap diffuse_texture;
    TextureMap specular_texture;
    TextureMap specular_exponent_texture;


    bool has_ambient_texture() const;
    bool has_diffuse_texture() const;
    bool has_specular_texture() const;
    bool has_specular_exponent_texture() const;
  };


  struct WavefrontObject {
    std::string material;
    std::vector<uint32_t> position_indices;
    std::vector<uint32_t> uv_indices;
    std::vector<uint32_t> normal_indices;
  };


  struct WavefrontMesh {
    std::vector<kmath::Vec3> positions;
    std::vector<kmath::Vec2> uvs;
    std::vector<kmath::Vec3> normals;
    std::map<std::string, WavefrontObject> objects;
    std::map<std::string, WavefrontMaterial> materials;

    static WavefrontMesh load(const std::filesystem::path &p_path);
  
  private:
    void _include_materials(const std::filesystem::path &p_path);
  };

}
