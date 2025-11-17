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


#include "wavefront_object.hpp"
#include "debug.hpp"

#include "thirdparty/kmath/vector.hpp"

#include <fstream>
#include <string>


namespace tputils {

  bool WavefrontMaterial::has_ambient_texture() const {
    return !ambient_texture.path.empty();
  }


  bool WavefrontMaterial::has_diffuse_texture() const {
    return !diffuse_texture.path.empty();
  }


  bool WavefrontMaterial::has_specular_texture() const {
    return !specular_texture.path.empty();
  }


  bool WavefrontMaterial::has_specular_exponent_texture() const {
    return !specular_exponent_texture.path.empty();
  }


  template<class e, class t>
  std::basic_istream<e,t>& operator>>(std::basic_istream<e,t>& in, const e& cliteral) { // FIXME: don't use that
      e buffer(0);
      in >> buffer;
      if (buffer != cliteral)
        in.setstate(std::ios::failbit); // Fail if the literal is not correct
      return in;
  }


  WavefrontMesh WavefrontMesh::load(const std::filesystem::path &p_path) {
    WavefrontMesh mesh{};
    
    std::ifstream file(p_path);
    WavefrontObject *current_object = nullptr;
    std::string token;

    while (file.good()) {
      token.clear();
      file >> token;
      
      if (token == "v") { // Vertex position
        if (!current_object) {
          LOG_WARNING("Could not load mesh at `" << p_path << "` missing object");
          return WavefrontMesh{};
        }
        float x, y, z;
        file >> x >> y >> z;
        mesh.positions.push_back(kmath::Vec3(x, y, z));
      } else if (token == "vn") { // Vertex normal
        if (!current_object) {
          LOG_WARNING("Could not load mesh at `" << p_path << "` missing object");
          return WavefrontMesh{};
        }
        float x, y, z;
        file >> x >> y >> z;
        mesh.normals.push_back(kmath::Vec3(x, y, z));
      } else if (token == "vt") { // Vertex texture
        if (!current_object) {
          LOG_WARNING("Could not load mesh at `" << p_path << "` missing object");
          return WavefrontMesh{};
        }
        float u, v;
        file >> u >> v;
        mesh.uvs.push_back(kmath::Vec2(u, v));
      } else if (token == "f") {
        if (!current_object) {
          LOG_WARNING("Could not load mesh at `" << p_path << "` missing object");
          return WavefrontMesh{};
        }
        uint32_t pos0, uv0, norm0, pos1, uv1, norm1, pos2, uv2, norm2;
        file >> pos0 >> '/' >> uv0 >> '/' >> norm0;
        file >> pos1 >> '/' >> uv1 >> '/' >> norm1;
        file >> pos2 >> '/' >> uv2 >> '/' >> norm2;

        current_object->position_indices.push_back(pos0 - 1);
        current_object->position_indices.push_back(pos1 - 1);
        current_object->position_indices.push_back(pos2 - 1);

        current_object->uv_indices.push_back(uv0 - 1);
        current_object->uv_indices.push_back(uv1 - 1);
        current_object->uv_indices.push_back(uv2 - 1);

        current_object->normal_indices.push_back(norm0 - 1);
        current_object->normal_indices.push_back(norm1 - 1);
        current_object->normal_indices.push_back(norm2 - 1);
      } else if (token == "o") { // Change the current object
        // Setup the new object
        file >> token;
        mesh.objects[token] = WavefrontObject{};
        current_object = &mesh.objects[token];
      } else if  (token == "mtllib") {
        std::filesystem::path material_path;
        file >> material_path;
        material_path = p_path.parent_path() / material_path;
        mesh._include_materials(material_path);
      } else if (token == "usemtl") {
        if (!current_object) {
          LOG_WARNING("Could not load mesh at `" << p_path << "` missing object");
          return WavefrontMesh{};
        }
        file >> current_object->material;
      } else if (token == "s" || token == "#") {
        std::getline(file, token); // Ignore tokens
      }
    }

    return mesh;
  }


  WavefrontMaterial::TextureMap _parse_texture_map(std::istream &p_stream) {
    WavefrontMaterial::TextureMap map = {};

    std::string token;

    while (p_stream.good()) {
      p_stream >> token;

      if (token == "-o") {
        p_stream >> map.origin_offset.x >> map.origin_offset.y >> map.origin_offset.z;
      } else if (token == "-s") {
        p_stream >> map.scale.x >> map.scale.y >> map.scale.z;
      } else if (token == "-blendu" || token == "-blendv" || token == "-bm" || token == "-boost" || token == "-cc"
        || token == "-clamp" || token == "-imfchan" || token == "-texres")
      { // discard options
        p_stream >> token;
      } else if (token == "-mm") {
        p_stream >> token >> token;
      } else if (token == "-t") {
        p_stream >> token >> token >> token;
      } else {
        map.path = token;
      }
    }

    return map;
  }


  void WavefrontMesh::_include_materials(const std::filesystem::path &p_path) {
    std::ifstream file(p_path);
    WavefrontMaterial *current_material = nullptr;
    std::string token;

    while (file.good()) {
      file >> token;

      if (current_material == nullptr && token != "newmtl" && token != "#") {
        LOG_WARNING("Trying to set material properties to non-existant material in `" << p_path << "`.");
        return;
      }

      if (token == "newmtl") {
        file >> token;
        materials[token] = WavefrontMaterial{};
        current_material = &materials[token];
      } else if (token == "illum") {
        unsigned int illum_model;
        file >> illum_model;
        current_material->illumination_model = (WavefrontMaterial::IlluminationModel)illum_model;
      } else if (token == "Ka") {
        file >> current_material->ambient_color.x >> current_material->ambient_color.y >> current_material->ambient_color.z;
      } else if (token == "Kd") {
        file >> current_material->diffuse_color.x >> current_material->diffuse_color.y >> current_material->diffuse_color.z;
      } else if (token == "Ks") {
        file >> current_material->specular_color.x >> current_material->specular_color.y >> current_material->specular_color.z;
      } else if (token == "Ns") {
        file >> current_material->specular_exponent;
      } else if (token == "Tr") {
        file >> current_material->dissolve;
        current_material->dissolve = 1.0 - current_material->dissolve;
      } else if (token == "d") {
        file >> current_material->dissolve;
      } else if (token == "Tf") {
        file >> current_material->transmission_filter_color.x >> current_material->transmission_filter_color.y >> current_material->transmission_filter_color.z;
      } else if (token == "Ni") {
        file >> current_material->index_of_refraction;
      } else if (token.substr(0, 4) == "map_") {
        std::string map_type = token.substr(4);
        WavefrontMaterial::TextureMap map = _parse_texture_map(file);
        map.path = p_path.parent_path() / map.path; // To a valid relative path

        if (map_type == "Ka") {
          current_material->ambient_texture = map;
        } else if (map_type == "Kd") {
          current_material->diffuse_texture = map;
        } else if (map_type == "Ks") {
          current_material->specular_texture = map;
        } else if (map_type == "Ns") {
          current_material->specular_exponent_texture = map;
        }
      } else if (token == "s" || token == "#") {
        std::getline(file, token); // Ignore tokens
      }
    }
  }
}
