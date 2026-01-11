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


#include "scene.hpp"
#include "geometry/ray.hpp"
#include "geometry/light.hpp"
#include "material.hpp"
#include "utils/renderer.hpp"

#include <optional>
#include <random>
#include <variant>


using namespace kmath;


RayIntersection Scene::compute_intersection(const Ray &p_ray) const {
  RayIntersection result;

  // Sphere intersection
  for (size_t i = 0; i < spheres.size(); i++) {
    RaySphereIntersection rsph = spheres[i].intersect(p_ray);
    if (rsph.exists && rsph.distance < result.intersection.common.distance) {
      result.intersection.rsph = rsph;
      result.element_id = i;
      result.kind = RayIntersection::Kind::RAY_SPHERE;
    }
  }

  // Square intersection
  for (size_t i = 0; i < squares.size(); i++) {
    RaySquareIntersection rsqu = squares[i].intersect(p_ray);
    if (rsqu.exists && rsqu.distance < result.intersection.common.distance) {
      result.intersection.rsqu = rsqu;
      result.element_id = i;
      result.kind = RayIntersection::Kind::RAY_SQUARE;
    }
  }

  // Mesh intersection
  for (size_t i = 0; i < meshes.size(); i++) {
    RayMeshIntersection rmsh = meshes[i].intersect(p_ray);
    if (rmsh.exists && rmsh.distance < result.intersection.common.distance) {
      result.intersection.rmsh = rmsh;
      result.element_id = i;
      result.kind = RayIntersection::Kind::RAY_MESH;
    }
  }

  return result;
}


Vec3 Scene::_intersection_get_color(std::mt19937 &p_rng, const Ray &p_ray, const RayIntersection &p_intersection) const {
  switch (p_intersection.kind) {
  case RayIntersection::Kind::RAY_SPHERE: {
    const Sphere &sphere = spheres[p_intersection.element_id];
    const RaySphereIntersection &rsph = p_intersection.intersection.rsph;
    return sphere.material.get_color(
      rsph.position,
      rsph.normal,
      -p_ray.direction,
      rsph.uv,
      0.1f * Lrgb::ONE,
      p_rng,
      lights
    );
  }
  case RayIntersection::Kind::RAY_SQUARE: {
    const Square &square = squares[p_intersection.element_id];
    const RaySquareIntersection &rsqu = p_intersection.intersection.rsqu;
    return square.material.get_color(
      rsqu.position,
      rsqu.normal,
      -p_ray.direction,
      rsqu.uv,
      0.1f * Lrgb::ONE,
      p_rng,
      lights
    );
  }
  case RayIntersection::Kind::RAY_MESH: {
    const Mesh &mesh = meshes[p_intersection.element_id];
    const RayMeshIntersection &rmsh = p_intersection.intersection.rmsh;
    return mesh.material.get_color(
      rmsh.position,
      rmsh.normal,
      -p_ray.direction,
      rmsh.uv,
      0.1f * Lrgb::ONE,
      p_rng,
      lights
    );
  }
  case RayIntersection::Kind::NONE:
    return Vec3::ZERO;
  }
  return Vec3::ZERO;
}


std::optional<const Material*> Scene::_intersection_get_material(const RayIntersection &p_intersection) const {
  switch (p_intersection.kind) {
  case RayIntersection::Kind::RAY_SPHERE: {
    const Sphere &sphere = spheres[p_intersection.element_id];
    return std::optional<const Material*>(&sphere.material);
  }
  case RayIntersection::Kind::RAY_SQUARE: {
    const Square &square = squares[p_intersection.element_id];
    return std::optional<const Material*>(&square.material);
  }
  case RayIntersection::Kind::RAY_MESH: {
    const Mesh &mesh = meshes[p_intersection.element_id];
    return std::optional<const Material*>(&mesh.material);
  }
  case RayIntersection::Kind::NONE:
    return std::optional<const Material*>();
  }
  return std::optional<const Material*>();
}


Lrgb Scene::ray_trace_recursive(std::mt19937 &p_rng, const Ray &p_ray, const int p_bounce_count) const {
  Lrgb color = Lrgb::ZERO;
  Ray ray = p_ray;
  float bounce_contribution = 1.0;

  for (int bounce = 0; bounce <= p_bounce_count; bounce++) {
    const RayIntersection scene_inter = compute_intersection(ray);

    if (!scene_inter.intersection.common.exists) {
      break;
    }

    // Apply lights
    const Material &intersection_material = *_intersection_get_material(scene_inter).value();
    const Vec3 intersection_normal = scene_inter.intersection.common.normal;
    const Vec3 intersection_point = scene_inter.intersection.common.position + 0.0001f * intersection_normal;
    const Vec2 intersection_uv = scene_inter.intersection.common.uv;
    Lrgb bounce_color = intersection_material.get_ambiant_contribution(intersection_uv);

    for (const Light &light : lights) {
      const Vec3 light_position = std::visit([&](const auto &p_shape) -> Vec3 { return p_shape(p_rng); }, light.shape);
      const Vec3 light_direction = light_position - intersection_point;
      const float light_distance = length(light_direction);
      const Ray light_ray = Ray(intersection_point, light_direction);

      const RayIntersection light_intersection = compute_intersection(light_ray);

      if (light_intersection.intersection.common.exists &&
          light_intersection.intersection.common.distance < light_distance) {
        continue;
      }

      bounce_color += intersection_material.get_light_influence(intersection_point, intersection_normal, ray.direction, intersection_uv, light.data, light_position);
    }

    // Add bounce contribution
    color += bounce_contribution * bounce_color;

    // Setup for the next light bounce
    const auto [bounce_direction, bounce_strength] = intersection_material.bounce(p_rng, ray.direction, intersection_normal);
    bounce_contribution *= bounce_strength;

    const float bounce_dir_sign = kmath::sign(kmath::dot(bounce_direction, intersection_normal));
    const Vec3 bounce_point = scene_inter.intersection.common.position + bounce_dir_sign * 0.0001f * intersection_normal;

    ray = Ray(bounce_point, bounce_direction);
  }
  
  return color;
}


Lrgb Scene::ray_trace(std::mt19937 &p_rng, const Ray &p_ray_start) const {
  RayIntersection inter = compute_intersection(p_ray_start);
  return _intersection_get_color(p_rng, p_ray_start, inter);
}


void Scene::draw() const {
  for(size_t i = 0 ; i < meshes.size() ; ++i) {
    const Mesh &mesh = meshes[i];
    mesh.draw();
  }
  for(size_t i = 0 ; i < spheres.size() ; ++i) {
    const Sphere &sphere = spheres[i];
    Renderer::get_singleton()->draw_sphere(sphere);
  }
  for(size_t i = 0 ; i < squares.size() ; ++i) {
    const Square &square = squares[i];
    Renderer::get_singleton()->draw_rect(square);
  }
}


// ====================
// = Scene definition =
// ====================

void Scene::setup_single_sphere() {
  meshes.clear();
  spheres.clear();
  squares.clear();
  lights.clear();
  {
    lights.resize(lights.size() + 1);
    Light &light = lights[lights.size() - 1];
    light.shape = UniformBallDistribution(Vec3(-5, 5, 5), 0.01f);
    light.data.radius = 2.5f;
    light.data.power_correction = 2.0f;
    light.data.color = Lrgb::ONE;
    light.data.energy = 8.0;
  }
  {
    spheres.resize(spheres.size() + 1);
    Sphere &s = spheres[spheres.size() - 1];
    s.center = Vec3(0. , 0. , 0.);
    s.radius = 1.f;
    s.material.diffuse = 1.0;
    s.material.mirror = 0.2;
    s.material.albedo = Vec3(1., 1., 1);
    s.material.shininess = 20;
  }
}


void Scene::setup_single_square() {
  meshes.clear();
  spheres.clear();
  squares.clear();
  lights.clear();
  {
    lights.resize(lights.size() + 1);
    Light &light = lights[lights.size() - 1];
    light.shape = UniformBallDistribution(Vec3(-5, 5, 5), 0.01f);
    light.data.radius = 2.5f;
    light.data.power_correction = 2.f;
    light.data.color = Lrgb::ONE;
    light.data.energy = 8.0;
  }
  {
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.material.albedo = Vec3(0.8, 0.8, 0.8);
    s.material.shininess = 20;
  }
}


void Scene::setup_cornell_box() {
  meshes.clear();
  spheres.clear();
  squares.clear();
  lights.clear();
  {
    lights.resize(lights.size() + 1);
    Light &light = lights[lights.size() - 1];
    // light.shape = UniformBallDistribution(Vec3(0.0, 1.5, 0.0), 0.1f);
    light.shape = PointDistribution(Vec3(0.0, 1.5, 0.0));
    light.data.radius = 2.0f;
    light.data.power_correction = 2.f;
    light.data.color = Lrgb::ONE;
    light.data.energy = 4.0;
  }
  { //Back Wall
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.scale(Vec3(2., 2., 1.));
    s.translate(Vec3(0., 0., -2.));
    s.material.albedo  = Vec3(0.6274509803921569, 0.1254901960784314, 0.9411764705882353);
    s.material.shininess = 16.0;
  }
  { //Left Wall
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.scale(Vec3(2., 2., 1.));
    s.translate(Vec3(0., 0., -2.));
    s.rotate_y(90);
    s.material.albedo = Vec3(1., 0., 0.);
    s.material.shininess = 16.0;
  }
  { //Right Wall
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.translate(Vec3(0., 0., -2.));
    s.scale(Vec3(2., 2., 1.));
    s.rotate_y(-90);
    s.rotate_x(180.0);
    // s.material.albedo = Vec3(0.0, 1.0, 0.0);
    s.material.albedo_tex = Image::read("assets/textures/sphere_textures/s1.ppm");
    s.material.diffuse = 0.2;
    s.material.specular = 0.05;
    s.material.shininess = 16.0;
  }
  { //Floor
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.translate(Vec3(0., 0., -2.));
    s.scale(Vec3(2., 2., 1.));
    s.rotate_x(-90);
    s.material.albedo = Vec3(1.0, 1.0, 1.0);
    s.material.shininess = 16.0;
  }
  { //Ceiling
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.translate(Vec3(0., 0., -2.));
    s.scale(Vec3(2., 2., 1.));
    s.rotate_x(90);
    s.material.albedo = Vec3(0.0, 0.0, 1.0);
    s.material.shininess = 16.0;
  }
  { //Front Wall
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.translate(Vec3(0., 0., -2.));
    s.scale(Vec3(2., 2., 1.));
    s.rotate_y(180);
    s.material.albedo = Vec3(1.0, 1.0, 1.0);
    s.material.shininess = 16.0;
  }
  { // GLASS Sphere
    spheres.resize(spheres.size() + 1);
    Sphere &s = spheres[spheres.size() - 1];
    s.center = Vec3(1.0, -1.0, 0.5);
    s.radius = 0.75f;
    s.material.diffuse = 0.04;
    s.material.mirror = 0.1;
    s.material.transparancy = 0.95;
    s.material.specular = 0.35;
    s.material.refractive_index = 1.1f;
    s.material.albedo = Vec3(0., 1., 1.);
    s.material.shininess = 16.0;
  }
  { // MIRRORED Sphere
    spheres.resize(spheres.size() + 1);
    Sphere &s = spheres[spheres.size() - 1];
    s.center = Vec3(-1.0, -1.0, -0.5);
    s.radius = 0.75f;
    s.material.diffuse = 0.4;
    s.material.mirror = 0.8;
    s.material.albedo = Vec3(1., 0., 0.);
    s.material.shininess = 16.0;
  }
  {
    // Mesh
    meshes.emplace_back();
    Mesh &mesh = meshes.back();
    mesh.load_obj("assets/models/unit_cube.obj");
    mesh.scale(0.5f * Vec3::ONE);
    mesh.translate(Vec3(-1.0, -1.0, 1.0));
    mesh.material.diffuse = 0.3;
    mesh.material.mirror = 0.1;
    mesh.material.albedo = Vec3(0.8, 0., 1.);
    mesh.material.shininess = 16.0;
    // mesh.load_obj("assets/models/unit_sphere.obj");
    // mesh.material.albedo_tex = Image::read("assets/textures/sphere_textures/s7.ppm");
  }
}


void Scene::setup_simple_mesh() {
  meshes.clear();
  spheres.clear();
  squares.clear();
  lights.clear();
  {
    lights.resize(lights.size() + 1);
    Light &light = lights[lights.size() - 1];
    // light.shape = UniformBallDistribution(Vec3(0.0, 1.5, 0.0), 0.1f);
    light.shape = PointDistribution(Vec3(0.0, 3.0, 0.0));
    light.data.radius = 2.0f;
    light.data.power_correction = 2.f;
    light.data.color = Lrgb::ONE;
    light.data.energy = 4.0;
  }
  {
    meshes.emplace_back();
    Mesh &mesh = meshes.back();
    mesh.load_obj("assets/models/unit_sphere.obj");
    mesh.rotate_z(180.0);
    mesh.build_acceleration_structure();
    mesh.material.diffuse = 1.0;
    mesh.material.albedo_tex = Image::read("assets/textures/sphere_textures/s1.ppm");
  }
}
