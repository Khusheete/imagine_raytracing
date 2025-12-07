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

#include "geometry/ray.hpp"
#include "thirdparty/kmath/vector.hpp"
#include "thirdparty/kmath/color.hpp"
#include "geometry/light.hpp"

#include <random>


struct Material {
public:
  kmath::Lrgb albedo = kmath::Lrgb::ONE;
  kmath::Lrgb specular = kmath::Lrgb::ZERO;
  float shininess = 1.0;

  float diffuse = 0.2f;
  float mirror = 0.0f;

  // float index_medium = 0.0f;
  // float transparency = 0.0f;

public:
  kmath::Lrgb get_light_influence(const kmath::Vec3 &p_fragment_position, const kmath::Vec3 &p_surface_normal, const kmath::Vec3 &p_camera_direction, const LightData &p_light_data, const kmath::Vec3 &p_light_position) const;
  
  template<typename LightIt, std::uniform_random_bit_generator Rng>
  kmath::Lrgb get_color(const kmath::Vec3 &p_fragment_position, const kmath::Vec3 &p_surface_normal, const kmath::Vec3 &p_camera_direction, const kmath::Lrgb &p_ambiant_energy, Rng &p_rng, const LightIt &p_lights) const {
    using namespace kmath;
  
    const Lrgb ambiant = albedo * p_ambiant_energy;

    Lrgb light_contribs = Lrgb::ZERO;
    for (const Light &light : p_lights) {
      const Vec3 light_position = std::visit([&](const auto &p_shape) -> Vec3 { return p_shape(p_rng); }, light.shape);
      light_contribs += get_light_influence(p_fragment_position, p_surface_normal, p_camera_direction, light.data, light_position);
    }

    return ambiant + light_contribs;
  }


  // Returns the direction of a bounce, alongside the strength of the bounced color.
  template<std::uniform_random_bit_generator Rng>
  std::pair<kmath::Vec3, float> bounce(Rng &p_rng, const kmath::Vec3 &p_ray_direction, const kmath::Vec3 &p_normal) const {
    using namespace kmath;

    const float total_weight = diffuse + mirror;
    std::uniform_real_distribution<float> selector(0.0f, total_weight);
    const float selection = selector(p_rng);

    if (selection < diffuse) {
      // This ray is to be bounced as a diffuse ray.
      const UniformHemishereDistribution bounce_distribution(p_normal);
      const kmath::Vec3 new_direction = bounce_distribution(p_rng);
      return std::pair(new_direction, diffuse);
    } else {
      // This ray is to be bounced as a reflected ray
      const Vec3 reflected_direction = reflect(p_ray_direction, p_normal);
      return std::pair(reflected_direction, mirror);
    }
  }
};


