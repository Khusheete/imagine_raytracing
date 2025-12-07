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


#include "rendering.hpp"

#include "debug.hpp"
#include "thirdparty/glad/include/glad/glad.h"

#include <GL/gl.h>
#include <GL/glext.h>

#include "thirdparty/glfw/include/GLFW/glfw3.h"
#include <iostream>
#include <sstream>


namespace tputils {

  void begin_frame(GLFWwindow *p_window, int &p_window_width, int &p_window_height) {
    // Listen for events
    glfwPollEvents();

    // Re-configure viewport
    glfwGetWindowSize(p_window, &p_window_width, &p_window_height);
    glViewport(0, 0, p_window_width, p_window_height);
    
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  }


  void end_frame(GLFWwindow *p_window) {
    // Present to screen
    glFlush();
    glfwSwapBuffers(p_window);
  }
  
  
  // ==================
  // = Init functions =
  // ==================


  void APIENTRY _gl_debug_context(GLenum p_source, GLenum p_type, GLuint p_id, GLenum p_severity, [[maybe_unused]] GLsizei p_length, const char *p_message, [[maybe_unused]] const void *_p_user_param) {
    // ignore non-significant error/warning codes
    if(p_id == 131169 || p_id == 131185 || p_id == 131218 || p_id == 131204) return;

    std::stringstream message;
    bool is_error = false;

    switch (p_type) {
      case GL_DEBUG_TYPE_ERROR:
        is_error = true;
        message << "OpenGL Error: "; break;
      case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        message << CSI_YELLOW << "OpenGL Warning - Deprecated Behaviour: "; break;
      case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        message << CSI_YELLOW << "OpenGL Warning - Undefined Behaviour: "; break; 
      case GL_DEBUG_TYPE_PORTABILITY:
        message << CSI_YELLOW << "OpenGL Warning - Portability: "; break;
      case GL_DEBUG_TYPE_PERFORMANCE:
        message << CSI_YELLOW << "OpenGL Warning - Performance: "; break;
      case GL_DEBUG_TYPE_MARKER:
        message << CSI_YELLOW << "OpenGL Warning - Marker: "; break;
      case GL_DEBUG_TYPE_PUSH_GROUP:
        message << CSI_YELLOW << "OpenGL Warning - Push Group: "; break;
      case GL_DEBUG_TYPE_POP_GROUP:
        message << CSI_YELLOW << "OpenGL Warning - Pop Group: "; break;
      case GL_DEBUG_TYPE_OTHER:
        message << CSI_YELLOW << "OpenGL Warning - Other: "; break;
    }

    switch (p_severity) {
      case GL_DEBUG_SEVERITY_HIGH:         message << "Severity: high; "; break;
      case GL_DEBUG_SEVERITY_MEDIUM:       message << "Severity: medium; "; break;
      case GL_DEBUG_SEVERITY_LOW:          message << "Severity: low; "; break;
      case GL_DEBUG_SEVERITY_NOTIFICATION: message << "Severity: notification; "; break;
    }

    switch (p_source) {
      case GL_DEBUG_SOURCE_API:             message << "Source: API; "; break;
      case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   message << "Source: Window System; "; break;
      case GL_DEBUG_SOURCE_SHADER_COMPILER: message << "Source: Shader Compiler; "; break;
      case GL_DEBUG_SOURCE_THIRD_PARTY:     message << "Source: Third Party; "; break;
      case GL_DEBUG_SOURCE_APPLICATION:     message << "Source: Application; "; break;
      case GL_DEBUG_SOURCE_OTHER:           message << "Source: Other; "; break;
    }

    message << "Debug message (" << p_id << "):\n" <<  p_message;

    if (!is_error) {
      LOG_WARNING(message.str());
    } else {
      LOG_ERROR(message.str());
    }
  }


  void init_opengl_context() {
    // Initialize glad to load OpenGL functions
    ASSERT_FATAL_ERROR(gladLoadGLLoader((GLADloadproc)&glfwGetProcAddress), "Could not initialize glad");

    // Print device information
    const uint8_t *opengl_version_info = glGetString(GL_VERSION);
    const uint8_t *hardware_info = glGetString(GL_RENDERER);
    std::cout << "OpenGL API " << opengl_version_info << " - Using: " << hardware_info << std::endl;

    int context_flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);

    // Add debug context
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(&_gl_debug_context, nullptr);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);

    // Default config
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    glClearColor(0.0903f, 0.0353f, 0.1294f, 1.0f);

    glPointSize(8.0f);
  }
}
