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


#include "camera.hpp"
#include "kmath/motor_3d.hpp"
#include "kmath/rotor_3d.hpp"


void FreeCamera::local_translate(const kmath::Vec3 &p_delta) {
  const kmath::Vec3 right = kmath::get_x_basis_vector(rotation);
  const kmath::Vec3 up = kmath::get_y_basis_vector(rotation);
  const kmath::Vec3 forward = -kmath::get_z_basis_vector(rotation);

  position += p_delta.x * right + p_delta.y * up + p_delta.z * forward;
}


void FreeCamera::local_rotate(const kmath::Vec2 &p_delta) {
  const kmath::Vec3 right = kmath::get_x_basis_vector(rotation);
  const kmath::Vec3 up = kmath::get_y_basis_vector(rotation);
  
  const kmath::Rotor3 vertical_rotate = kmath::Rotor3::from_axis_angle(right, -p_delta.y);
  kmath::Rotor3 horizontal_rotate = kmath::Rotor3::from_axis_angle(kmath::Vec3::Y, p_delta.x);
  if (kmath::dot(kmath::Vec3::Y, up)) {
    horizontal_rotate = kmath::reverse(horizontal_rotate);
  }

  rotation = kmath::normalized(
    horizontal_rotate * vertical_rotate * rotation
  );
}


kmath::Mat4 FreeCamera::get_view_matrix() const {
  return kmath::as_transform(
    kmath::reverse(kmath::Motor3::from_rotor_translation(rotation, position))
  );
}


kmath::Mat4 FreeCamera::get_inv_view_matrix() const {
  return kmath::as_transform(
    rotation, position
  );
}
