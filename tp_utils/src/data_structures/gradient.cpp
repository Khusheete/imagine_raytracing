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


#include "gradient.hpp"
#include "thirdparty/kmath/color.hpp"
#include "thirdparty/kmath/interpolation_functions.hpp"
#include "thirdparty/kmath/utils.hpp"



using namespace kmath;


namespace tputils {
  
  void Gradient::add_point(const Lrgb &p_color, const float p_offset) {
    const OkLab oklab_color = lrgb_to_oklab(p_color);

    if (points.empty()) {
      points.push_back({oklab_color, p_offset});
      return;
    }
    
    for (size_t i = 0; i < points.size(); i++) {
      if (points[i].offset > p_offset) {
        points.insert(points.begin() + i, {oklab_color, p_offset});
        return;
      }
    }

    points.push_back({oklab_color, p_offset});
  }


  Lrgb Gradient::sample(const float p_at) const {
    if (p_at < points.front().offset || points.back().offset < p_at) {
      return outside_color;
    }

    size_t index = 1;
    while (get_point_offset(index) < p_at) {
      index += 1;
    }

    const float mix = inv_lerp(get_point_offset(index - 1), get_point_offset(index), p_at);
    const OkLab color = lerp(points[index - 1].color, points[index].color, mix);
    return oklab_to_lrgb(color);
  }


  void Gradient::set_point_offset(const size_t p_index, const float p_offset) {
    points[p_index].offset = p_offset;

    if (p_index > 0 && p_offset < get_point_offset(p_index - 1)) {
      size_t new_index = p_index - 1;
      while (new_index > 0 && p_offset < get_point_offset(new_index)) {
        new_index -= 1;
      }

      Point lift = points[new_index];
      points[new_index] = points[p_index];
      for (size_t i = new_index + 1; i <= p_index; i++) {
        Point tmp = points[i];
        points[i] = lift;
        lift = tmp;
      }
    } else if (p_index + 1 < get_point_count() && p_offset > get_point_offset(p_index + 1)) {
      const size_t size = get_point_count();
      size_t new_index = p_index + 1;
      while (new_index + 1 < size && p_offset > get_point_offset(new_index)) {
        new_index += 1;
      }

      for (size_t i = p_index; i < new_index; i++) {
        Point tmp = points[i];
        points[i] = points[i + 1];
        points[i + 1] = tmp;
      }
    }
  }
}
