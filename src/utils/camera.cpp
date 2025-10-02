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
