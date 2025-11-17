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
#include "thirdparty/kmath/euclidian_flat_3d.hpp"
#include "thirdparty/kmath/motor_3d.hpp"
#include "thirdparty/kmath/rotor_3d.hpp"
#include "thirdparty/kmath/vector.hpp"
#include "thirdparty/kmath/angles.hpp"

#include <algorithm>


using namespace kmath;


namespace tputils {

  Mat4 Camera2D::get_projection_matrix(const float p_screen_width, const float p_screen_height) const {
    return Mat4::orthogonal_lh_no_ndc(0.01, 1.0, p_screen_width, p_screen_height);
  }


  Mat4 Camera2D::get_view_matrix() const {
    return Mat4::from_basis(
      Mat3::z_rotation(-rotation),
      Vec3(-position, -0.01)
    );
  }


  Mat4 Camera3D::get_projection_matrix(const float p_aspect_ratio) const {
    switch (projection) {
    case Projection::PERSPECTIVE:
      return Mat4::perspective_lh_no_ndc_vfov(near_plane, far_plane, vertical_opening, p_aspect_ratio);
    case Projection::ORTHOGONAL:
      return Mat4::orthogonal_lh_no_ndc(near_plane, far_plane, vertical_opening * p_aspect_ratio, vertical_opening);
    }
  }


  Mat4 Camera3D::get_view_matrix() const {
    return as_transform(inverse(Motor3::from_rotor_translation(rotation, position)));
  }


  Mat4 Camera3D::get_inverse_view_matrix() const {
    return as_transform(Motor3::from_rotor_translation(rotation, position));
  }


  void Camera3D::rotate(const Rotor3 &p_by) {
    rotation = p_by * rotation;
  }


  void Camera3D::transform(const Motor3 &p_by) {
    Motor3 motor = Motor3::from_rotor_translation(rotation, position);
    motor = p_by * motor;
    rotation = normalized(get_rotor(motor));
    position = get_translation(motor);
  }


  // See reference implementation: https://github.com/quakeforge/quakeforge/blob/master/ruamoko/qwaq/qwaq-ed.r#L550-L609
  // FIXME: Does not work for p_up = -Vec3::Y
  void Camera3D::look_at(const kmath::Vec3 &p_position, const kmath::Point3 &p_center, const kmath::Point3 &p_up) {
    const Point3 center = (is_vanishing(p_center))? p_center : p_center - Point3::direction(p_position);

    const Line3 forward_line = Line3::line(-Vec3::Z);
    const Plane3 view_plane = Plane3::YZ;
    Line3 new_forward_line = join(Point3::ORIGIN, center);
    Plane3 new_view_plane = join(new_forward_line, p_up);

    constexpr float MIN_UP_UNALIGNMENT = 0.005;
    const float up_unalignment = inner(new_view_plane, new_view_plane) / inner(new_forward_line, reverse(new_forward_line));
    if (up_unalignment < MIN_UP_UNALIGNMENT) {
      // We are looking into the (unsigned) up direction.
      // The idea is to fall back to the reference view_plane (YZ).
      // FIXME: A bit finicky when vertical
      const float orient = sign(new_view_plane.e1);
      const float t = up_unalignment / MIN_UP_UNALIGNMENT;
      new_view_plane = lerp(orient * view_plane, new_view_plane, t);
    }
    new_forward_line = normalized(new_forward_line);
    new_view_plane = normalized(new_view_plane);
    
    Motor3 trans;
    const Motor3 line_align = normalized(new_forward_line / forward_line);

    if (line_align.s < -0.5) {
      // We are looking away from center along the -Z direction.
      // The square root would give us a 180° rotated transformation along the up direction (Y).
      // We need to undo that rotation.
      trans = sqrt(line_align);
      trans = Rotor3::from_axis_angle(get_y_basis_vector(get_real_part(trans)), PI) * trans;
    } else {
      trans = sqrt(line_align);
    }

    const Motor3 plane_align = normalized(new_view_plane / kmath::transform(view_plane, trans));

    if (plane_align.s < -0.5) {
      // The target alignment plane is opposite from our view plane.
      // The square root would give us a 180° rotated transformation along the up direction (Y).
      // We need to undo that rotation.
      trans = sqrt(plane_align) * trans;
      trans = Rotor3::from_axis_angle(get_y_basis_vector(get_real_part(trans)), PI) * trans;
    } else {
      trans = sqrt(plane_align) * trans;
    }

    trans = normalized(trans);

    rotation = get_real_part(trans);
    position = p_position;
  }


  void Camera3D::normalize() {
    rotation = normalized(rotation);
  }


  Camera3D::Camera3D(const Projection p_proj): projection(p_proj) {}


  Camera3D::Camera3D(const Vec3 &p_position, const Rotor3 &p_rotation, const Projection p_proj)
  : rotation(p_rotation), position(p_position), projection(p_proj) {}


  void FreeCamera3D::update(const Vec3 &p_position_delta, const Vec2 &p_rotation_delta) {
    const Mat3 basis = as_basis(rotation);
    translate(basis * p_position_delta);
    
    const Rotor3 vertical_rotate = Rotor3::from_axis_angle(basis.x, -p_rotation_delta.y);
    const Rotor3 horizontal_rotate = Rotor3::from_axis_angle(Vec3::Y, -p_rotation_delta.x);

    rotation = normalized(horizontal_rotate * vertical_rotate * rotation);
  }


  void TrackballCamera3D::set_target(const Vec3 &p_target) {
    target = p_target;
    _recompute_transformation();
  }


  void TrackballCamera3D::set_angle(const Vec2 &p_angle) {
    angle = p_angle;
    _clamp_angle();
    _recompute_transformation();
  }


  void TrackballCamera3D::set_radius(const float p_radius) {
    radius = std::abs(p_radius);
    _recompute_transformation();
  }


  void TrackballCamera3D::set_trackball_position(const Vec3 &p_target, const Vec2 &p_angle, const float p_radius) {
    target = p_target;
    angle = p_angle;
    _clamp_angle();
    radius = std::abs(p_radius);
    _recompute_transformation();
  }


  void TrackballCamera3D::update(const kmath::Vec2 &p_angle_delta, const float p_radius_delta) {
    set_trackball_position(target, angle + p_angle_delta, radius + p_radius_delta);
  }


  inline void TrackballCamera3D::_clamp_angle() {
    angle.x = std::clamp(angle.x, 0.15f, (float)(PI - 0.15f));
  }


  void TrackballCamera3D::_recompute_transformation() {
    position = kmath::spherical_to_cartesian(radius, angle.x, angle.y);
    look_at(position, Point3::point(target), Point3::Y_DIR);
  }

  TrackballCamera3D::TrackballCamera3D() {
    _recompute_transformation();
  }
}
