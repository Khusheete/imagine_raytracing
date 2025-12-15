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


#include "image.hpp"

#include "thirdparty/kmath/color.hpp"
#include "thirdparty/stb/stb_image.h"
#include <algorithm>
#include <iostream>


Image::Image(const size_t p_width, const size_t p_height, const kmath::Lrgb p_fill)
  : data(p_width * p_height, p_fill),
  width(p_width),
  height(p_height)
{}


void Image::resize(const size_t p_new_width, const size_t p_new_height) {
  width = p_new_width;
  height = p_new_height;
  data.resize(get_size());
}


Image Image::read(const std::filesystem::path &p_path) {
  int width, height, components;
  float *image_data = stbi_loadf(p_path.c_str(), &width, &height, &components, 3);
  Image result(width, height);
  if (components != 3) {
    std::cout << components << std::endl;
    std::cout << "Invalid image" << std::endl;
    return result;
  }

  for (size_t i = 0; i < result.get_size(); i++) {
    result(i) = kmath::Lrgb(
      image_data[3 * i + 0],
      image_data[3 * i + 1],
      image_data[3 * i + 2]
    );
  }

  stbi_image_free(image_data);
  return result;
}


void Image::write_ppm(std::ostream &p_stream, const size_t p_precision) const {
  p_stream << "P3" << std::endl << width << " " << height << std::endl << p_precision << std::endl;
  for (size_t i = 0; i < data.size(); i++) {
    p_stream << static_cast<int>(p_precision * std::clamp(data[i].x, 0.0f, 1.0f)) << " ";
    p_stream << static_cast<int>(p_precision * std::clamp(data[i].y, 0.0f, 1.0f)) << " ";
    p_stream << static_cast<int>(p_precision * std::clamp(data[i].z, 0.0f, 1.0f)) << "\n";
  }
  p_stream << std::endl;
}


kmath::Lrgb Image::sample(const float p_u, const float p_v, const SampleMode p_sample_mode) const {
  switch (p_sample_mode) {
  break;case SampleMode::NEAREST: {
    const size_t x = std::round(p_u * (width - 1));
    const size_t y = std::round(p_v * (height - 1));
    return (*this)(x, y);
  }
  break;case SampleMode::LINEAR: {
    const float x = p_u * (width - 1);
    const float y = p_v * (height - 1);
    const size_t x_coord = std::floor(x);
    const size_t y_coord = std::floor(y);
    const float x_part = std::fmod(x, (float)1.0);
    const float y_part = std::fmod(y, (float)1.0);
    
    const kmath::Lrgb c00 = (*this)(x_coord + 0, y_coord + 0);
    const kmath::Lrgb c01 = (*this)(x_coord + 0, y_coord + 1);
    const kmath::Lrgb c10 = (*this)(x_coord + 1, y_coord + 0);
    const kmath::Lrgb c11 = (*this)(x_coord + 1, y_coord + 1);

    const kmath::Lrgb ct = kmath::lerp(c00, c01, x_part);
    const kmath::Lrgb cb = kmath::lerp(c10, c11, x_part);
    return kmath::lerp(ct, cb, y_part);
  }
  }
  return kmath::Lrgb::ZERO;
}


kmath::Lrgb Image::sample(const kmath::Vec2 p_uv, const SampleMode p_sample_mode) const {
  return sample(p_uv.x, p_uv.y, p_sample_mode);
}
