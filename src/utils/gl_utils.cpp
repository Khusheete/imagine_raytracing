#include "gl_utils.hpp"

#include <GL/gl.h>


using namespace kmath;


Mat4 get_modelview_matrixf() {
  Mat4 result;
  glGetFloatv(GL_MODELVIEW_MATRIX, reinterpret_cast<GLfloat*>(&result));
  return result;
}


Mat4 get_projection_matrixf() {
  Mat4 result;
  glGetFloatv(GL_PROJECTION_MATRIX, reinterpret_cast<GLfloat*>(&result));
  return result;
}


Vec2 get_depth_range() {
  Vec2 result;
  glGetFloatv(GL_DEPTH_RANGE, reinterpret_cast<GLfloat*>(&result));
  return result;
}
