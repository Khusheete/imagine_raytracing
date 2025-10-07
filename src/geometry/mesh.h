#ifndef MESH_H
#define MESH_H


#include <vector>
#include <string>
#include <cfloat>
#include <cstdint>

#include <GL/gl.h>

#include "kmath/matrix.hpp"
#include "kmath/vector.hpp"

#include "ray.h"
#include "material.h"



// -------------------------------------------
// Basic Mesh class
// -------------------------------------------


struct MeshVertex {
  kmath::Vec3 position;
  kmath::Vec3 normal;
  kmath::Vec2 uv;
};


struct MeshTriangle {
  uint32_t a = 0, b = 0, c = 0;

public:
  inline unsigned int &operator[](unsigned int iv) { return *(&a + iv); }
  inline unsigned int operator[](unsigned int iv) const { return *(&a + iv); }
};




class Mesh {
public:
  Material material;

  void load_off(const std::string & filename);
  void recompute_normals();
  void center_and_scale_to_unit();
  void scale_unit();

  virtual void build_arrays();

  void scale(const kmath::Vec3 &p_scale);
  void rotate_x(const float p_angle);
  void rotate_y(const float p_angle);
  void rotate_z(const float p_angle);
  virtual void translate(const kmath::Vec3 &p_translation);
  virtual void apply_transformation_matrix(const kmath::Mat3 &p_transform);

  void draw() const;

  RayTriangleIntersection intersect(const Ray &p_ray) const;


  virtual ~Mesh() = default;


protected:
  void build_positions_array();
  void build_normals_array();
  void build_UVs_array();
  void build_triangles_array();


protected:
  std::vector<MeshVertex> vertices;
  std::vector<MeshTriangle> triangles;

  std::vector<float> positions_array;
  std::vector<float> normals_array;
  std::vector<float> uvs_array;
  std::vector<unsigned int> triangles_array;
};


#endif
