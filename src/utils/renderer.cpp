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


#include "renderer.hpp"
#include "thirdparty/kmath/matrix.hpp"
// #include "tp_utils/src/rendering/immediate_geometry.hpp"
#include "tp_utils/src/debug.hpp"
#include "tp_utils/src/model_loaders/wavefront_object.hpp"
#include "tp_utils/src/rendering/immediate_geometry.hpp"
#include "tp_utils/src/rendering/mesh.hpp"
#include "tp_utils/src/rendering/primitives/shader.hpp"
#include "tp_utils/src/source_preprocessing.hpp"


using namespace tputils;
using namespace kmath;


// Init the renderer singleton to nullptr
Renderer *Renderer::singleton = nullptr;


void Renderer::set_color(const kmath::Lrgb &p_color) {
  object_shader.bind_uniform("u_color", p_color);
}


void Renderer::draw_sphere(const Sphere &p_sphere) {
  object_shader.bind_uniform(
    "u_model",
    Mat4::translation(p_sphere.center) * Mat4::scale(p_sphere.radius)
  );
  set_color(p_sphere.material.albedo);
  sphere.draw();
}


void Renderer::draw_rect(const Square &p_square) {
  object_shader.bind_uniform("u_model", Mat4::IDENTITY);
  set_color(p_square.material.albedo);
  imgeo.begin(ImmediateGeometry::Mode::TRIANGLES, &object_layout);

  const Vec3 bl = p_square.bottom_left;
  const Vec3 br = p_square.bottom_left + p_square.size.x * p_square.right_vector;
  const Vec3 tl = p_square.bottom_left + p_square.size.y * p_square.up_vector;
  const Vec3 tr = br + p_square.size.y * p_square.up_vector;

  const Vec2 bl_uv = p_square.uv_min;
  const Vec2 br_uv = Vec2(p_square.uv_max.x, p_square.uv_min.y);
  const Vec2 tl_uv = Vec2(p_square.uv_min.x, p_square.uv_max.y);
  const Vec2 tr_uv = p_square.uv_max;

  imgeo.push_vec3(bl);
  imgeo.push_vec3(p_square.normal);
  imgeo.push_vec2(bl_uv);

  imgeo.push_vec3(br);
  imgeo.push_vec3(p_square.normal);
  imgeo.push_vec2(br_uv);

  imgeo.push_vec3(tl);
  imgeo.push_vec3(p_square.normal);
  imgeo.push_vec2(tl_uv);

  imgeo.push_vec3(tl);
  imgeo.push_vec3(p_square.normal);
  imgeo.push_vec2(tl_uv);

  imgeo.push_vec3(br);
  imgeo.push_vec3(p_square.normal);
  imgeo.push_vec2(br_uv);

  imgeo.push_vec3(tr);
  imgeo.push_vec3(p_square.normal);
  imgeo.push_vec2(tr_uv);
  
  imgeo.end();
}


void Renderer::set_projection_view_matrix(const kmath::Mat4 &p_projection_view) {
  object_shader.bind_uniform("u_projection_view", p_projection_view);
}


void Renderer::set_model_matrix(const kmath::Mat4 &p_model) {
  object_shader.bind_uniform("u_model", p_model);
}


void Renderer::begin_frame() {
  object_shader.bind();
}


void Renderer::end_frame() {
  object_shader.unbind();
}


void Renderer::init_singleton() {
  singleton = new Renderer();

  singleton->sphere = TriangleMesh::from_wavefront_mesh(
    WavefrontMesh::load("assets/models/unit_sphere.obj")
  );

  { // Load shader
    tputils::SourceFile sf("assets/shaders/forward.glsl");
    singleton->object_shader = tputils::ShaderProgram(
      sf.get_section_text("vertex"),
      sf.get_section_text("fragment")
    );
    ASSERT_FATAL_ERROR(
      singleton->object_shader.is_valid(),
      singleton->object_shader.get_errors()
    );
  }
}


void Renderer::clean_singleton() {
  delete singleton;
  singleton = nullptr;
}
