#include "scene.h"
#include "geometry/ray.h"
#include "light.hpp"
#include "material.h"
#include <optional>


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

  return result;
}


Vec3 Scene::_intersection_get_color(const Ray &p_ray, const RayIntersection &p_intersection) const {
  switch (p_intersection.kind) {
  case RayIntersection::Kind::RAY_SPHERE: {
    const Sphere &sphere = spheres[p_intersection.element_id];
    const RaySphereIntersection &rsph = p_intersection.intersection.rsph;
    return sphere.material.get_color(
      rsph.position,
      rsph.normal,
      -p_ray.direction,
      0.1f * Lrgb::ONE,
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
      0.1f * Lrgb::ONE,
      lights
    );
  }
  case RayIntersection::Kind::RAY_TRIANGLE:
    return Vec3::ZERO;
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
  case RayIntersection::Kind::RAY_TRIANGLE:
    return std::optional<const Material*>();
  case RayIntersection::Kind::NONE:
    return std::optional<const Material*>();
  }
  return std::optional<const Material*>();
}


Lrgb Scene::ray_trace_recursive(const Ray &p_ray, const int p_bounce_count) const {
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
    Lrgb bounce_color = Lrgb::ZERO;

    for (const Light &light : lights) {
      const Vec3 light_direction = light.pos - intersection_point;
      const float light_distance = length(light_direction);
      const Ray light_ray = Ray(intersection_point, light_direction);

      const RayIntersection light_intersection = compute_intersection(light_ray);

      if (light_intersection.intersection.common.exists &&
          light_intersection.intersection.common.distance < light_distance) {
        continue;
      }

      bounce_color += intersection_material.get_light_influence(intersection_point, intersection_normal, ray.direction, light);
    }

    // Add bounce contribution
    color += bounce_contribution * bounce_color;

    // Setup for the next light bounce
    bounce_contribution *= 0.2;
    const Vec3 reflected_direction = reflect(ray.direction, intersection_normal);
    ray = Ray(intersection_point, reflected_direction);
  }
  
  return color;
}


Lrgb Scene::ray_trace(const Ray &p_ray_start) const {
  RayIntersection inter = compute_intersection(p_ray_start);
  return _intersection_get_color(p_ray_start, inter);
}


void Scene::draw() const {
  for(size_t i = 0 ; i < meshes.size() ; ++i) {
    const Mesh &mesh = meshes[i];
    mesh.draw();
  }
  for(size_t i = 0 ; i < spheres.size() ; ++i) {
    const Sphere &sphere = spheres[i];
    sphere.draw();
  }
  for(size_t i = 0 ; i < squares.size() ; ++i) {
    const Square &square = squares[i];
    square.draw();
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
    light.pos = Vec3(-5,5,5);
    light.radius = 2.5f;
    light.power_correction = 2.0f;
    // light.type = LightType::SPHERICAL;
    light.color = Lrgb::ONE;
    light.energy = 20.0;
  }
  {
    spheres.resize(spheres.size() + 1);
    Sphere &s = spheres[spheres.size() - 1];
    s.center = Vec3(0. , 0. , 0.);
    s.radius = 1.f;
    s.build_arrays();
    s.material.type = MaterialType::MIRROR;
    s.material.diffuse_material = Vec3(1., 1., 1);
    s.material.specular_material = Vec3(0.2, 0.2, 0.2);
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
    light.pos = Vec3(-5, 5, 5);
    light.radius = 2.5f;
    light.power_correction = 2.f;
    // light.type = LightType::SPHERICAL;
    light.color = Lrgb::ONE;
    light.energy = 20.0;
  }
  {
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.build_arrays();
    s.material.diffuse_material = Vec3(0.8, 0.8, 0.8);
    s.material.specular_material = Vec3(0.8, 0.8, 0.8);
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
    light.pos = Vec3(0.0, 1.5, 0.0);
    light.radius = 2.5f;
    light.power_correction = 2.f;
    // light.type = LightType::SPHERICAL;
    light.color = Lrgb::ONE;
    light.energy = 20.0;
  }
  { //Back Wall
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.scale(Vec3(2., 2., 1.));
    s.translate(Vec3(0., 0., -2.));
    s.build_arrays();
    s.material.diffuse_material  = Vec3(0.6274509803921569, 0.1254901960784314, 0.9411764705882353);
    s.material.specular_material = Vec3(0.6274509803921569, 0.1254901960784314, 0.9411764705882353);
    s.material.shininess = 16;
  }
  { //Left Wall
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.scale(Vec3(2., 2., 1.));
    s.translate(Vec3(0., 0., -2.));
    s.rotate_y(90);
    s.build_arrays();
    s.material.diffuse_material = Vec3(1., 0., 0.);
    s.material.specular_material = Vec3(1., 0., 0.);
    s.material.shininess = 16;
  }
  { //Right Wall
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.translate(Vec3(0., 0., -2.));
    s.scale(Vec3(2., 2., 1.));
    s.rotate_y(-90);
    s.build_arrays();
    s.material.diffuse_material = Vec3(0.0, 1.0, 0.0);
    s.material.specular_material = Vec3(0.0, 1.0, 0.0);
    s.material.shininess = 16;
  }
  { //Floor
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.translate(Vec3(0., 0., -2.));
    s.scale(Vec3(2., 2., 1.));
    s.rotate_x(-90);
    s.build_arrays();
    s.material.diffuse_material = Vec3(1.0, 1.0, 1.0);
    s.material.specular_material = Vec3(1.0, 1.0, 1.0);
    s.material.shininess = 16;
  }
  { //Ceiling
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.translate(Vec3(0., 0., -2.));
    s.scale(Vec3(2., 2., 1.));
    s.rotate_x(90);
    s.build_arrays();
    s.material.diffuse_material = Vec3(0.0, 0.0, 1.0);
    s.material.specular_material = Vec3(0.0, 0.0, 1.0);
    s.material.shininess = 16;
  }
  { //Front Wall
    squares.resize(squares.size() + 1);
    Square &s = squares[squares.size() - 1];
    s.set_quad(Vec3(-1., -1., 0.), Vec3(1., 0, 0.), Vec3(0., 1, 0.), Vec2(2.0, 2.0));
    s.translate(Vec3(0., 0., -2.));
    s.scale(Vec3(2., 2., 1.));
    s.rotate_y(180);
    s.build_arrays();
    s.material.diffuse_material = Vec3(1.0, 1.0, 1.0);
    s.material.specular_material = Vec3(1.0, 1.0, 1.0);
    s.material.shininess = 16;
  }
  { //GLASS Sphere
    spheres.resize(spheres.size() + 1);
    Sphere &s = spheres[spheres.size() - 1];
    s.center = Vec3(1.0, -1.25, 0.5);
    s.radius = 0.75f;
    s.build_arrays();
    s.material.type = MaterialType::MIRROR;
    s.material.diffuse_material = Vec3(1., 0., 0.);
    s.material.specular_material = Vec3(1., 0., 0.);
    s.material.shininess = 16;
    s.material.transparency = 1.0;
    s.material.index_medium = 1.4;
  }
  { //MIRRORED Sphere
    spheres.resize(spheres.size() + 1);
    Sphere &s = spheres[spheres.size() - 1];
    s.center = Vec3(-1.0, -1.25, -0.5);
    s.radius = 0.75f;
    s.build_arrays();
    s.material.type = MaterialType::GLASS;
    s.material.diffuse_material = Vec3(0., 1., 1.);
    s.material.specular_material = Vec3(0., 1., 1.);
    s.material.shininess = 16;
    s.material.transparency = 0.;
    s.material.index_medium = 0.;
  }
}
