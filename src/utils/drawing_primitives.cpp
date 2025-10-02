#include "drawing_primitives.hpp"
#include "kmath/motor_3d.hpp"
#include "kmath/rotor_3d.hpp"
#include "kmath/vector.hpp"

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

