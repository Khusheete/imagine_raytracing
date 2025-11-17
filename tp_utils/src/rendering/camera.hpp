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


#include "thirdparty/kmath/constants.hpp"
#include "thirdparty/kmath/euclidian_flat_3d.hpp"
#include "thirdparty/kmath/matrix.hpp"
#include "thirdparty/kmath/motor_3d.hpp"
#include "thirdparty/kmath/rotor_3d.hpp"
#include "thirdparty/kmath/vector.hpp"


namespace tputils {

  struct Camera2D {
    kmath::Vec2 position;
    float rotation;

  public:
    kmath::Mat4 get_projection_matrix(const float p_screen_width, const float p_screen_height) const;
    kmath::Mat4 get_view_matrix() const;
    inline kmath::Mat4 get_projection_view_matrix(const float p_screen_width, const float p_screen_height) const { return get_projection_matrix(p_screen_width, p_screen_height) * get_view_matrix(); }
  };


  class Camera3D {
  public:
    enum class Projection : bool {
      PERSPECTIVE,
      ORTHOGONAL,
    };


  public:
    kmath::Mat4 get_projection_matrix(const float p_aspect_ratio) const;
    kmath::Mat4 get_view_matrix() const;
    kmath::Mat4 get_inverse_view_matrix() const;
    inline kmath::Mat4 get_projection_view_matrix(const float p_aspect_ratio) const { return get_projection_matrix(p_aspect_ratio) * get_view_matrix(); }
    inline kmath::Vec3 get_forward_direction() const { return -kmath::get_z_basis_vector(rotation); }

    void rotate(const kmath::Rotor3 &p_by);
    inline void translate(const kmath::Vec3 &p_by) { position += p_by; }
    void transform(const kmath::Motor3 &p_by);

    void look_at(const kmath::Vec3 &p_position, const kmath::Point3 &p_center, const kmath::Point3 &p_up);

    void normalize();

    inline void set_rotation(const kmath::Rotor3 &p_new_rot) { rotation = p_new_rot; }
    inline void set_position(const kmath::Vec3 &p_new_pos) { position = p_new_pos; }
    inline void set_projection(const Projection p_proj) { projection = p_proj; }
    inline void set_near_plane(const float p_near) { near_plane = p_near; }
    inline void set_far_plane(const float p_far) { far_plane = p_far; }
    inline void set_vfov(const float p_vfov) { vertical_opening = p_vfov; }
    inline void set_height(const float p_height) { vertical_opening = p_height; }
    
    inline kmath::Rotor3 get_rotation() const { return rotation; }
    inline kmath::Vec3 get_position() const { return position; }
    inline Projection get_projection() const { return projection; }
    inline float get_near_plane() const { return near_plane; }
    inline float get_far_plane() const { return far_plane; }
    inline float get_vfov() const { return vertical_opening; }
    inline float get_height() const { return vertical_opening; }

  public:
    Camera3D() = default;
    Camera3D(const Projection p_proj);
    Camera3D(const kmath::Vec3 &p_position, const kmath::Rotor3 &p_rotation, const Projection p_proj);

  protected:
    kmath::Rotor3 rotation = kmath::Rotor3::IDENTITY;
    kmath::Vec3 position = kmath::Vec3::Z;
    float near_plane = 0.1;
    float far_plane = 20.0;
    float vertical_opening = 75.0f * kmath::PI / 180.0f; // The vertical fov for a perspective camera, and the height of the window for an orthogonal one
    Projection projection = Projection::PERSPECTIVE;
  };


  class FreeCamera3D : public Camera3D {
  public:

    void update(const kmath::Vec3 &p_position_delta, const kmath::Vec2 &p_rotation_delta);
  };


  class TrackballCamera3D : public Camera3D {
  public:
    void set_target(const kmath::Vec3 &p_target);
    void set_angle(const kmath::Vec2 &p_angle);
    void set_radius(const float p_radius);
    void set_trackball_position(const kmath::Vec3 &p_target, const kmath::Vec2 &p_angle, const float p_radius);

    inline kmath::Vec3 get_target() const { return target; }
    inline kmath::Vec2 get_angle() const { return angle; }
    inline float get_radius() const { return radius; }

    void update(const kmath::Vec2 &p_angle_delta, const float p_radius_delta);

    TrackballCamera3D();

  protected:
    inline void _clamp_angle();
    void _recompute_transformation();

  protected:
    kmath::Vec3 target = kmath::Vec3::ZERO;
    kmath::Vec2 angle = kmath::Vec2(0.5f * kmath::PI, 0.0f);
    float radius = 5.0;
  };
}
