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


#include "drawing_primitives.hpp"
#include "thirdparty/kmath/motor_3d.hpp"
#include "thirdparty/kmath/rotor_3d.hpp"
#include "thirdparty/kmath/vector.hpp"

#include <GL/gl.h>


using namespace kmath;


// =====================
// = Drawing functions =
// =====================


void load_matrix(const Mat4 &p_mat) {
  glLoadMatrixf(reinterpret_cast<const float*>(&p_mat));
}


void mult_matrix(const Mat4 &p_matrix) {
  glMultMatrixf(reinterpret_cast<const float*>(&p_matrix));
}


void load_transformation(const kmath::Motor3 &p_transformation) {
  load_matrix(as_transform(p_transformation));
}


void load_transformation(const kmath::Rotor3 &p_rotation, const kmath::Vec3 &p_translation) {
  load_matrix(as_transform(p_rotation, p_translation));
}


void push_transformation(const Rotor3 &p_rotation, const Vec3 &p_translation) {
  glPushMatrix();
  mult_matrix(as_transform(p_rotation, p_translation));
}


void push_transformation(const Motor3 &p_transformation) {
  glPushMatrix();
  mult_matrix(as_transform(p_transformation));
}


void pop_transformation() {
  glPopMatrix();
}

