#include "scene.h"
#include "geometry/ray.h"


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


Vec3 Scene::ray_trace_recursive(const Ray &p_ray, const int p_remaining_bounces) const {
  //TODO RaySceneIntersection raySceneIntersection = computeIntersection(ray);
  Vec3 color;
  return color;
}


Vec3 Scene::ray_trace(const Ray &p_ray_start) const {
  RayIntersection inter = compute_intersection(p_ray_start);

  Vec3 color;

  switch (inter.kind) {
  case RayIntersection::Kind::RAY_SPHERE: {
    const Sphere &sphere = spheres[inter.element_id];
    const RaySphereIntersection &rsph = inter.intersection.rsph;
    color = sphere.material.get_color(
      rsph.position,
      rsph.normal,
      -p_ray_start.direction,
      0.1f * Lrgb::ONE,
      lights
    );
      break;
  }
  case RayIntersection::Kind::RAY_SQUARE: {
    const Square &square = squares[inter.element_id];
    const RaySquareIntersection &rsqu = inter.intersection.rsqu;
    color = square.material.get_color(
      rsqu.position,
      rsqu.normal,
      -p_ray_start.direction,
      0.1f * Lrgb::ONE,
      lights
    );
      break;
  }
  case RayIntersection::Kind::RAY_TRIANGLE:
    break;
  case RayIntersection::Kind::NONE:
    return Vec3::ZERO;
  }

  // for (const Light &light : lights) {
  //   // TODO
  // }

  return color;
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
