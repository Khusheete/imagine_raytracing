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
#include <algorithm>


Image::Image(const size_t p_width, const size_t p_height, const kmath::Lrgb p_fill)
  : data(p_width * p_height, p_fill),
  width(p_width),
  height(p_height)
{}


Image Image::copy() const {
  Image image{};
  image.data = data;
  image.width = width;
  image.height = height;
  return image;
}


void Image::write_ppm(std::ostream &p_stream) const {
  p_stream << "P3" << std::endl << width << " " << height << std::endl << 255 << std::endl;
  for (size_t i = 0; i < data.size(); i++) {
    p_stream << static_cast<int>(255.0f * std::clamp(data[i].x, 0.0f, 1.0f)) << " ";
    p_stream << static_cast<int>(255.0f * std::clamp(data[i].y, 0.0f, 1.0f)) << " ";
    p_stream << static_cast<int>(255.0f * std::clamp(data[i].z, 0.0f, 1.0f)) << "\n";
  }
  p_stream << std::endl;
}
