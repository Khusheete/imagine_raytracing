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


#include "material.h"


using namespace kmath;


Lrgb Material::get_light_influence(const Vec3 &p_fragment_position, const Vec3 &p_surface_normal, const Vec3 &p_camera_direction, const Light &p_light) const {
  const Vec3 light_direction = normalized(p_light.pos - p_fragment_position);
  
  const float signed_light_direction = dot(p_surface_normal, light_direction);

  if (signed_light_direction > 0.0) {
    const float light_distance = distance(p_light.pos, p_fragment_position);
    const float light_attenuation = std::pow(1.0 / light_distance, p_light.power_correction);
    
    const float diffuse_contrib = std::max(0.0f, signed_light_direction);
    const Vec3 diffuse = light_attenuation * p_light.energy * diffuse_contrib * p_light.color;

    const Vec3 reflected_dir = 2.0f * signed_light_direction * p_surface_normal - light_direction;
    const float specular_contrib = std::pow(std::max(0.0f, dot(p_camera_direction, reflected_dir)), shininess);
    const Vec3 specular = light_attenuation * p_light.energy * specular_contrib * specular_material * p_light.color;

    return diffuse * diffuse_material + specular * specular_material;
  }

  return Lrgb::ZERO;
}
