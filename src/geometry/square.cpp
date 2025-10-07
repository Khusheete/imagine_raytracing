#include "square.h"



void Square::set_quad(const kmath::Vec3 &p_bottom_left, const kmath::Vec3 &p_right_vector, const kmath::Vec3 &p_up_vector, const kmath::Vec2 &p_size, const kmath::Vec2 &p_uv_min, const kmath::Vec2 &p_uv_max) {
  m_right_vector = kmath::normalized(p_right_vector);
  m_up_vector = kmath::normalized(p_up_vector);
  m_normal = kmath::normalized(kmath::cross(p_right_vector, p_up_vector));
  m_bottom_left = p_bottom_left;

  m_right_vector = m_right_vector * p_size.x;
  m_up_vector = m_up_vector * p_size.y;

  vertices.clear();
  vertices.resize(4);
  vertices[0].position = p_bottom_left;                                      vertices[0].uv = kmath::Vec2(p_uv_min.x, p_uv_max.x);
  vertices[1].position = p_bottom_left + m_right_vector;                     vertices[1].uv = kmath::Vec2(p_uv_max.y, p_uv_max.x);
  vertices[2].position = p_bottom_left + m_right_vector + m_up_vector;       vertices[2].uv = kmath::Vec2(p_uv_max.y, p_uv_max.y);
  vertices[3].position = p_bottom_left + m_up_vector;                        vertices[3].uv = kmath::Vec2(p_uv_min.x, p_uv_max.y);
  vertices[0].normal = vertices[1].normal = vertices[2].normal = vertices[3].normal = m_normal;

  triangles.clear();
  triangles.resize(2);
  triangles[0][0] = 0;
  triangles[0][1] = 1;
  triangles[0][2] = 2;
  triangles[1][0] = 0;
  triangles[1][1] = 2;
  triangles[1][2] = 3;
}


RaySquareIntersection Square::intersect(const Ray &p_ray) const {
  RaySquareIntersection intersection;

  //TODO calculer l'intersection rayon quad

  return intersection;
}


Square::Square(): Mesh() {}


Square::Square(const kmath::Vec3 &p_bottom_left, const kmath::Vec3 &p_right_vector, const kmath::Vec3 &p_up_vector, const kmath::Vec2 &p_size, const kmath::Vec2 &p_uv_min, const kmath::Vec2 &p_uv_max) : Mesh() {
  set_quad(p_bottom_left, p_right_vector, p_up_vector, p_size, p_uv_min, p_uv_max);
}
