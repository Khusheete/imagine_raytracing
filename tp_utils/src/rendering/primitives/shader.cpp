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


#include "shader.hpp"

#include "tp_utils/src/debug.hpp"

#include "vbuffer.hpp"
#include "thirdparty/kmath/vector.hpp"
#include "thirdparty/kmath/matrix.hpp"

#include <sstream>
#include <vector>



namespace tputils {
  void ShaderProgram::bind() const {
    glUseProgram(program_id);
  }


  bool ShaderProgram::is_bound() const {
    int pid;
    glGetIntegerv(GL_CURRENT_PROGRAM, &pid);
    return ((uint32_t)pid) == program_id;
  }


  void ShaderProgram::bind_uniform(const std::string &p_uniform_name, const float &p_value) {
    int32_t location = _get_uniform_location(p_uniform_name);
    if (location == -1) return;
    glUniform1f(location, p_value);
  }
  
  
  void ShaderProgram::bind_uniform(const std::string &p_uniform_name, const kmath::Vec2 &p_value) {
    int32_t location = _get_uniform_location(p_uniform_name);
    if (location == -1) return;
    glUniform2f(location, p_value.x, p_value.y);
  }
  
  
  void ShaderProgram::bind_uniform(const std::string &p_uniform_name, const kmath::Vec3 &p_value) {
    int32_t location = _get_uniform_location(p_uniform_name);
    if (location == -1) return;
    glUniform3f(location, p_value.x, p_value.y, p_value.z);
  }
  
  
  void ShaderProgram::bind_uniform(const std::string &p_uniform_name, const kmath::Vec4 &p_value) {
    int32_t location = _get_uniform_location(p_uniform_name);
    if (location == -1) return;
    glUniform4f(location, p_value.x, p_value.y, p_value.z, p_value.w);
  }
  
  
  void ShaderProgram::bind_uniform(const std::string &p_uniform_name, const int &p_value) {
    int32_t location = _get_uniform_location(p_uniform_name);
    if (location == -1) return;
    glUniform1i(location, p_value);
  }
  
  
  void ShaderProgram::bind_uniform(const std::string &p_uniform_name, const kmath::Vec2i &p_value) {
    int32_t location = _get_uniform_location(p_uniform_name);
    if (location == -1) return;
    glUniform2i(location, p_value.x, p_value.y);
  }
  
  
  void ShaderProgram::bind_uniform(const std::string &p_uniform_name, const kmath::Vec3i &p_value) {
    int32_t location = _get_uniform_location(p_uniform_name);
    if (location == -1) return;
    glUniform3i(location, p_value.x, p_value.y, p_value.z);
  }
  
  
  void ShaderProgram::bind_uniform(const std::string &p_uniform_name, const kmath::Vec4i &p_value) {
    int32_t location = _get_uniform_location(p_uniform_name);
    if (location == -1) return;
    glUniform4i(location, p_value.x, p_value.y, p_value.z, p_value.w);
  }
  
  
  void ShaderProgram::bind_uniform(const std::string &p_uniform_name, const kmath::Mat2 &p_value) {
    int32_t location = _get_uniform_location(p_uniform_name);
    if (location == -1) return;
    glUniformMatrix2fv(location, 1, GL_FALSE, reinterpret_cast<const float*>(&p_value));
  }
  
  
  void ShaderProgram::bind_uniform(const std::string &p_uniform_name, const kmath::Mat3 &p_value) {
    int32_t location = _get_uniform_location(p_uniform_name);
    if (location == -1) return;
    glUniformMatrix3fv(location, 1, GL_FALSE, reinterpret_cast<const float*>(&p_value));
  }
  
  
  void ShaderProgram::bind_uniform(const std::string &p_uniform_name, const kmath::Mat4 &p_value) {
    int32_t location = _get_uniform_location(p_uniform_name);
    if (location == -1) return;
    glUniformMatrix4fv(location, 1, GL_FALSE, reinterpret_cast<const float*>(&p_value));
  }


  void ShaderProgram::bind_storage_buffer(const std::string &p_storage_buffer_name, const VBuffer::Slice &p_slice) {
    int32_t location = _get_storage_buffer_location(p_storage_buffer_name);
    if (location == -1) return;
    p_slice.bind_to_location(location);
  }


  int32_t ShaderProgram::_get_uniform_location(const std::string &p_name) {
    return glGetUniformLocation(program_id, p_name.c_str());
  }


  int32_t ShaderProgram::_get_storage_buffer_location(const std::string &p_storage_buffer_name) {
    int32_t resource_index = glGetProgramResourceIndex(program_id, GL_SHADER_STORAGE_BLOCK, p_storage_buffer_name.data());
    uint32_t prop = GL_BUFFER_BINDING;
    int32_t location;
    glGetProgramResourceiv(program_id, GL_SHADER_STORAGE_BLOCK, resource_index, 1, &prop, 1, nullptr, &location);
    return location;
  }


  void ShaderProgram::unbind() const {
    glUseProgram(0);
  }


  int ShaderProgram::get_uniform_count() {
    return uniforms.size();
  }
  
  
  int ShaderProgram::get_uniform_index(const std::string &p_uniform_name) {
    for (size_t i = 0; i < uniforms.size(); i++) {
      if (uniforms[i].name == p_uniform_name) return i;
    }
    return -1;
  }
  
  
  const std::string &ShaderProgram::get_uniform_name(int index) {
    ASSERT_FATAL_ERROR(index >= 0 && index < get_uniform_count(), "Error index out of bounds");
    return uniforms[index].name;
  }


  ShaderDataType ShaderProgram::get_uniform_type(int index) {
    ASSERT_FATAL_ERROR(index >= 0 && index < get_uniform_count(), "Error index out of bounds");
    return uniforms[index].type;
  }


  int ShaderProgram::get_storage_buffer_count() {
    return storage_buffers.size();
  }
  
  
  int ShaderProgram::get_storage_buffer_index(const std::string &p_storage_buffer_name) {
    for (size_t i = 0; i < storage_buffers.size(); i++) {
      if (storage_buffers[i].name == p_storage_buffer_name) return i;
    }
    return -1;
  }
  
  
  const std::string &ShaderProgram::get_storage_buffer_name(int p_index) {
    ASSERT_FATAL_ERROR(p_index >= 0 && p_index < get_storage_buffer_count(), "Error index out of bounds");
    return storage_buffers[p_index].name;
  }


  bool ShaderProgram::is_valid() const {
    return !creation_error && program_id != 0;
  }


  const std::string &ShaderProgram::get_errors() const {
    return errors;
  }


  void ShaderProgram::load_program(const std::string &p_vertex_source, const std::string &p_fragment_source) {
    // Reset shader values
    if (program_id != 0) glDeleteProgram(program_id);
    program_id = 0;
    uniforms.clear();
    storage_buffers.clear();
    creation_error = false;

    std::stringstream error_stream;

    // Compile vertex shader
    const char *cvertex_source = p_vertex_source.c_str();
    uint32_t vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &cvertex_source, nullptr);
    glCompileShader(vertex_shader);

    int32_t ok;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
      creation_error = true;
      int32_t max_length;
      glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &max_length);
      std::vector<GLchar> error_logs(max_length);
      glGetShaderInfoLog(vertex_shader, max_length, &max_length, error_logs.data());
      error_stream << "Error while compiling vertex shader:" << std::endl;
      error_stream << error_logs.data() << std::endl;
    }

    // Compile fragment shader
    const char *cfragment_source = p_fragment_source.c_str();
    uint32_t fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &cfragment_source, nullptr);
    glCompileShader(fragment_shader);

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
      creation_error = true;
      int32_t max_length;
      glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &max_length);
      std::vector<GLchar> error_logs(max_length);
      glGetShaderInfoLog(fragment_shader, max_length, &max_length, error_logs.data());
      error_stream << "Error while compiling fragment shader:" << std::endl;
      error_stream << error_logs.data() << std::endl;
    }

    // Create shader program
    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);
    glLinkProgram(program_id);

    glGetProgramiv(program_id, GL_LINK_STATUS, &ok);
    if (!ok) {
      creation_error = true;
      int32_t max_length;
      glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &max_length);
      std::vector<GLchar> error_logs(max_length);
      glGetProgramInfoLog(program_id, max_length, &max_length, error_logs.data());
      error_stream << "Error while linking shader program:" << std::endl;
      error_stream << error_logs.data() << std::endl;    
    }

    // Free used shaders
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);

    // Get shader inputs information
    int32_t uniform_count;
    glGetProgramiv(program_id, GL_ACTIVE_UNIFORMS, &uniform_count);
    uniforms.reserve(uniform_count);
    int32_t max_uniform_name_length;
    glGetProgramiv(program_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_length);

    int32_t ssb_count;
    glGetProgramInterfaceiv(program_id, GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &ssb_count);
    storage_buffers.reserve(ssb_count);
    int32_t max_ssb_name_length;
    glGetProgramInterfaceiv(program_id, GL_SHADER_STORAGE_BLOCK, GL_MAX_NAME_LENGTH, &max_ssb_name_length);

    int32_t max_name_length = std::max(max_uniform_name_length, max_ssb_name_length);
    std::vector<GLchar> name(max_name_length);

    for (int i = 0; i < uniform_count; i++) {
      int32_t size;
      uint32_t type;
      glGetActiveUniform(program_id, i, max_name_length, nullptr, &size, &type, name.data());
      
      uniforms.push_back(Uniform{
        .type = shader_data_type_from_gl_type(type),
        .name = std::string(name.data())
      });
    }

    for (int i = 0; i < ssb_count; i++) {
      int32_t size;
      glGetProgramResourceName(program_id, GL_SHADER_STORAGE_BLOCK, i, max_name_length, &size, name.data());

      storage_buffers.push_back(StorageBuffer{
        .name = std::string(name.data())
      });
    }

    // Record possible errors
    if (creation_error) errors = error_stream.str();
  }


  ShaderProgram::ShaderProgram(const std::string &p_vertex_source, const std::string &p_fragment_source) {
    load_program(p_vertex_source, p_fragment_source);
  }


  ShaderProgram::ShaderProgram(ShaderProgram &&p_other)
    : creation_error(p_other.creation_error),
    errors(std::move(p_other.errors))
  {
    if (program_id != 0) glDeleteProgram(program_id);

    p_other.program_id = 0;
  }


  ShaderProgram &ShaderProgram::operator=(ShaderProgram &&p_other) {
    if (program_id != 0) glDeleteProgram(program_id);
    
    program_id = p_other.program_id;
    p_other.program_id = 0;
    
    return *this;
  }


  ShaderProgram::~ShaderProgram() {
    if (program_id != 0) {
      glDeleteProgram(program_id);
    }
  }


  // ShaderProgram ShaderProgram::load_shader_program(const std::string &p_vertex_path, const std::string &p_fragment_path) {
  //   return ShaderProgram(_read_file(p_vertex_path), _read_file(p_fragment_path));
  // }
}
