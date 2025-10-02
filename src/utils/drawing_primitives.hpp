#pragma once

#include "kmath/vector.hpp"
#include "kmath/motor_3d.hpp"

#include <GL/gl.h>


void load_matrix(const kmath::Mat4 &p_mat);
void mult_matrix(const kmath::Mat4 &p_matrix);
void push_transformation(const kmath::Motor3 &p_transformation);
void push_transformation(const kmath::Rotor3 &p_rotation, const kmath::Vec3 &p_translation);
void load_transformation(const kmath::Motor3 &p_transformation);
void load_transformation(const kmath::Rotor3 &p_rotation, const kmath::Vec3 &p_translation);
void pop_transformation();
