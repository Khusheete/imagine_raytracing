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


#include "off_object.hpp"

#include "debug.hpp"
#include "rendering/primitives/shader.hpp"
#include "rendering/primitives/vbuffer.hpp"

#include <array>
#include <fstream>
#include <vector>


using namespace kmath;


namespace tputils {
  OffObject OffObject::load(const std::filesystem::path &p_path, const bool p_load_normals) {
    ASSERT_FATAL_ERROR(!p_load_normals, "Loading normals in OFF files is not supported yet.");

    std::ifstream file(p_path);

    {
      std::string magic;
      file >> magic;
      if (magic.compare("OFF")) {
        LOG_WARNING("File `" << p_path << "` is not an OFF object");
        return {};
      }
    }

    size_t vertex_count, face_count, edge_count;
    file >> vertex_count >> face_count >> edge_count;
    
    // Setup
    OffObject result;
    result.positions.reserve(vertex_count);
    // if (p_load_normals) {
    //   result.normals.reserve(vertex_count);
    // }
    result.tris.reserve(face_count);
    
    // Load vertices
    for (size_t i = 0; i < vertex_count; i++) {
      float x, y, z;
      file >> x >> y >> z;
      result.positions.emplace_back(x, y, z);
    }

    // Load faces
    for (size_t i = 0; i < face_count; i++) {
      uint32_t element_count, a, b, c;
      file >> element_count;

      if (element_count != 3) [[unlikely]] {
        LOG_WARNING("Encountered unsupported " << element_count << "-gon while loading OFF file: `" << p_path << "`");
        return {};
      }

      file >> a >> b >> c;
      result.tris.emplace_back(a, b, c);
    }

    return result;
  }
}

