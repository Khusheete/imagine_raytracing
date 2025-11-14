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


#ifndef MATERIAL_H
#define MATERIAL_H

#include "kmath/vector.hpp"
#include "kmath/color.hpp"
#include "light.hpp"

#include <cmath>


enum class MaterialType : unsigned char {
  DIFFUSE_BLINN_PHONG,
  GLASS,
  MIRROR
};


struct Material {
public:
  kmath::Lrgb diffuse_material = kmath::Lrgb::ONE;
  kmath::Lrgb specular_material = kmath::Lrgb::ZERO;
  double shininess = 1.0;

  float index_medium = 0.0f;
  float transparency = 0.0f;

  MaterialType type = MaterialType::DIFFUSE_BLINN_PHONG;

public:
  kmath::Lrgb get_light_influence(const kmath::Vec3 &p_fragment_position, const kmath::Vec3 &p_surface_normal, const kmath::Vec3 &p_camera_direction, const Light &p_light) const;
  
  template<typename LightIt>
  kmath::Lrgb get_color(const kmath::Vec3 &p_fragment_position, const kmath::Vec3 &p_surface_normal, const kmath::Vec3 &p_camera_direction, const kmath::Lrgb &p_ambiant_energy, const LightIt &p_lights) const {
    using namespace kmath;
  
    const Lrgb ambiant = diffuse_material * p_ambiant_energy;

    Lrgb light_contribs = Lrgb::ZERO;
    for (const Light &light : p_lights) {
      light_contribs += get_light_influence(p_fragment_position, p_surface_normal, p_camera_direction, light);
    }

    return ambiant + light_contribs;
  }
};




#endif // MATERIAL_H
