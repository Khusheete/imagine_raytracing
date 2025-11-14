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


#ifndef SQUARE_H
#define SQUARE_H

#include "mesh.h"

#include "kmath/vector.hpp"

#include <cmath>


class Square : public Mesh {
public:
  void set_quad(const kmath::Vec3 &p_bottom_left, const kmath::Vec3 &p_right_vector, const kmath::Vec3 &p_up_vector, const kmath::Vec2 &p_size = kmath::Vec2::ONE, const kmath::Vec2 &p_uv_min = kmath::Vec2::ZERO, const kmath::Vec2 &p_uv_max = kmath::Vec2::ZERO);
  RaySquareIntersection intersect(const Ray &p_ray) const;

  virtual void translate(const kmath::Vec3 &p_translation) override;
  virtual void apply_transformation_matrix(const kmath::Mat3 &p_transform) override;

  Square();
  Square(const kmath::Vec3 &p_bottom_left, const kmath::Vec3 &p_right_vector, const kmath::Vec3 &p_up_vector, const kmath::Vec2 &p_size = kmath::Vec2::ONE, const kmath::Vec2 &p_uv_min = kmath::Vec2::ZERO, const kmath::Vec2 &p_uv_max = kmath::Vec2::ZERO);
  virtual ~Square() = default;

private:
  kmath::Vec3 normal;
  kmath::Vec3 bottom_left;
  kmath::Vec3 right_vector;
  kmath::Vec3 up_vector;
  kmath::Vec2 size;
};


#endif // SQUARE_H
