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


#ifndef SCENE_H
#define SCENE_H

#include <optional>
#include <vector>

#include "geometry/ray.h"
#include "geometry/mesh.h"
#include "geometry/sphere.h"
#include "geometry/square.h"
#include "material.h"


class Scene {
  std::vector<Mesh> meshes;
  std::vector<Sphere> spheres;
  std::vector<Square> squares;
  std::vector<Light> lights;

public:
  void draw() const;

  RayIntersection compute_intersection(const Ray &p_ray) const;
  kmath::Lrgb ray_trace_recursive(const Ray &p_ray, const int p_bounce_count = 4) const;
  kmath::Lrgb ray_trace(const Ray &p_ray_start) const;
  
  void setup_single_sphere();
  void setup_single_square();
  void setup_cornell_box();

public:
  kmath::Lrgb _intersection_get_color(const Ray &p_ray, const RayIntersection &p_intersection) const;
  std::optional<const Material*> _intersection_get_material(const RayIntersection &p_intersection) const;
};



#endif // #ifndef SCENE_H
