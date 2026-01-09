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
#include "thirdparty/kmath/euclidian_flat_3d.hpp"
#include "thirdparty/kmath/motor_3d.hpp"
#include "thirdparty/kmath/rotor_3d.hpp"
#include "thirdparty/kmath/vector.hpp"
#include "thirdparty/kmath/color.hpp"
#include "geometry/light.hpp"
#include "utils/image.hpp"

#include <optional>
#include <random>


struct Material {
public:
  kmath::Lrgb albedo = kmath::Lrgb::ONE;
  float shininess = 1.0;

  float diffuse = 0.3f;
  float specular = 0.2f;
  float mirror = 0.0f;
  float transparancy = 0.0f;

  float refractive_index = 1.458f;
  std::optional<Image> albedo_tex;

public:
  kmath::Lrgb get_light_influence(const kmath::Vec3 &p_fragment_position, const kmath::Vec3 &p_surface_normal, const kmath::Vec3 &p_camera_direction, const kmath::Vec2 &p_uv, const LightData &p_light_data, const kmath::Vec3 &p_light_position) const;
  
  template<typename LightIt, std::uniform_random_bit_generator Rng>
  kmath::Lrgb get_color(const kmath::Vec3 &p_fragment_position, const kmath::Vec3 &p_surface_normal, const kmath::Vec3 &p_camera_direction, const kmath::Vec2 &p_uv, const kmath::Lrgb &p_ambiant_energy, Rng &p_rng, const LightIt &p_lights) const {
    using namespace kmath;
  
    const Lrgb ambiant = albedo * p_ambiant_energy;

    Lrgb light_contribs = Lrgb::ZERO;
    for (const Light &light : p_lights) {
      const Vec3 light_position = std::visit([&](const auto &p_shape) -> Vec3 { return p_shape(p_rng); }, light.shape);
      light_contribs += get_light_influence(p_fragment_position, p_surface_normal, p_camera_direction, p_uv, light.data, light_position);
    }

    return ambiant + light_contribs;
  }


  // Returns the direction of a bounce, alongside the strength of the bounced color.
  template<std::uniform_random_bit_generator Rng>
  std::pair<kmath::Vec3, float> bounce(Rng &p_rng, const kmath::Vec3 &p_ray_direction, const kmath::Vec3 &p_normal) const {
    using namespace kmath;

    const float diffuse_cutoff = diffuse;
    const float mirror_cutoff = diffuse + mirror;

    const float total_weight = diffuse + mirror + transparancy;

    std::uniform_real_distribution<float> selector(0.0f, total_weight);
    const float selection = selector(p_rng);

    if (selection < diffuse_cutoff) {
      // This ray is to be bounced as a diffuse ray.
      const UniformHemishereDistribution bounce_distribution(p_normal);
      const kmath::Vec3 new_direction = bounce_distribution(p_rng);
      return std::pair(new_direction, diffuse);
    } else if (selection < mirror_cutoff) {
      // This ray is to be bounced as a reflected ray
      const Vec3 reflected_direction = reflect(p_ray_direction, p_normal);
      return std::pair(reflected_direction, mirror);
    } else {
      // This ray is to be bounced as a mirrored ray
      constexpr float AIR_REFRACTIVE_INDEX = 1.000293f;

      float incident_refractive_index;
      float medium_refractive_index;
      Vec3 refraction_normal;

      if (kmath::dot(p_ray_direction, p_normal) <= 0.0f) {
        incident_refractive_index = AIR_REFRACTIVE_INDEX;
        medium_refractive_index = refractive_index;
        refraction_normal = -p_normal;
      } else {
        incident_refractive_index = refractive_index;
        medium_refractive_index = AIR_REFRACTIVE_INDEX;
        refraction_normal = p_normal;
      }

      const kmath::Line3 axis = kmath::meet(
        Plane3::plane(refraction_normal, 0.0f),
        Plane3::plane(p_ray_direction, 0.0f)
      );;

      const float sin_incident_angle = kmath::magnitude(axis);
      const float sin_refracted_angle = incident_refractive_index * sin_incident_angle / medium_refractive_index;

      if (sin_refracted_angle > 1.0f) {
        // The ray cannot be bounced, it is reflected
        const Vec3 reflected_direction = reflect(p_ray_direction, p_normal);
        return std::pair(reflected_direction, transparancy);
      }

      const kmath::Rotor3 rotation = kmath::get_real_part(kmath::exp(sin_refracted_angle * kmath::normalized(axis)));

      return std::pair(
        kmath::transform(refraction_normal, rotation),
        transparancy
      );
    }
  }
};


