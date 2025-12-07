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


#include "thirdparty/kmath/utils.hpp"
#include "thirdparty/kmath/vector.hpp"

#include <random>


struct PointDistribution {
  kmath::Vec3 position;

public:
  template<std::uniform_random_bit_generator Rng>
  kmath::Vec3 operator()([[maybe_unused]] Rng &p_rng) const {
    return position;
  }
};


struct UniformCuboidDistribution {
  kmath::Vec3 min;
  kmath::Vec3 max;

public:
  template<std::uniform_random_bit_generator Rng>
  kmath::Vec3 operator()([[maybe_unused]] Rng &p_rng) const {
    return kmath::apply([&](const float x, const float y) -> float {
      return std::uniform_real_distribution<float>(x, y)(p_rng);
    }, min, max);
  }
};


struct UniformCubeDistribution {
  float min;
  float max;

public:
  template<std::uniform_random_bit_generator Rng>
  kmath::Vec3 operator()([[maybe_unused]] Rng &p_rng) const {
    std::uniform_real_distribution distr(min, max);
    return kmath::Vec3(
      distr(p_rng),
      distr(p_rng),
      distr(p_rng)
    );
  }
};


struct UniformBallDistribution {
  kmath::Vec3 position;
  float radius;

public:
  template<std::uniform_random_bit_generator Rng>
  kmath::Vec3 operator()(Rng &p_rng) const {
    const float r2 = radius * radius;
    UniformCubeDistribution distr(-radius, radius);
    while (true) {
      kmath::Vec3 vec = distr(p_rng);
      if (kmath::length_squared(vec) < r2) {
        return vec;
      }
    }
  }
};


struct UniformRectangleDistribution {
  kmath::Vec3 position;
  kmath::Vec3 right_vector;
  kmath::Vec3 up_vector;

public:
  template<std::uniform_random_bit_generator Rng>
  kmath::Vec3 operator()(Rng &p_rng) const {
    std::uniform_real_distribution distr(0.0f, 1.0f);
    return position + distr(p_rng) * right_vector + distr(p_rng) * up_vector;
  }
};


struct UniformHemishereDistribution {
  kmath::Vec3 normal;

public:
  template<std::uniform_random_bit_generator Rng>
  kmath::Vec3 operator()(Rng &p_rng) const {
    const UniformBallDistribution ball(kmath::Vec3::ZERO, 1.0f);
    const kmath::Vec3 result = ball(p_rng);
    const float sign = kmath::possign(dot(result, normal));
    return sign * result;
  }
};
