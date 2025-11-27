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


#include "tp_utils/src/rendering/mesh.hpp"
#include "tp_utils/src/rendering/primitives/shader.hpp"
#include "tp_utils/src/rendering/immediate_geometry.hpp"

#include "geometry/sphere.hpp"
#include "geometry/square.hpp"
#include "geometry/mesh.hpp"
#include "geometry/light.hpp"
#include "tp_utils/src/rendering/primitives/vbuffer.hpp"

#include <array>
#include <span>


class Renderer {
public:
  inline tputils::ImmediateGeometry &immediate_geometry() { return imgeo; }
  inline const tputils::BufferLayout *get_default_buffer_layout() const { return &object_layout; }

  void begin_frame();
  void end_frame();

  // TODO: implement
  void set_lights(std::span<const Light> &p_lights);

  void set_projection_view_matrix(const kmath::Mat4 &p_projection_view);
  void set_model_matrix(const kmath::Mat4 &p_model);

  void draw_sphere(const Sphere &p_sphere);
  void draw_rect(const Square &p_square);
  void draw_mesh(const Mesh &p_mesh);

public:
  static void init_singleton();
  static void clean_singleton();
  static inline Renderer *get_singleton() { return singleton; }

private:
  Renderer() = default;

private:
  tputils::ImmediateGeometry imgeo;
  std::array<tputils::ShaderDataType, 3> object_layout_types = {
    tputils::ShaderDataType::Vec3, tputils::ShaderDataType::Vec3, tputils::ShaderDataType::Vec2
  };
  tputils::BufferLayout object_layout{object_layout_types};

  tputils::ShaderProgram object_shader;

  tputils::TriangleMesh sphere;
  
private:
  static Renderer *singleton;
};
