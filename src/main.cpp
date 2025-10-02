#include <GL/gl.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <chrono>

#include <algorithm>

#include <GLFW/glfw3.h>

#include "kmath/constants.hpp"
#include "kmath/vector.hpp"
#include "utils/camera.hpp"
#include "scene.h"
#include "utils/drawing_primitives.hpp"
#include "utils/gl_utils.hpp"


// #include "imageLoader.h"
// #include "Material.h"


// ====================
// = Global variables =
// ====================


// Windowing

static GLFWwindow *window;
static unsigned int window_width = 480;
static unsigned int window_height = 480;
static unsigned int frame_rate = 0;
static bool fullscreen = false;

static long prev_frame_time;
static float frame_delta;
static long frame_count;


// Camera and inputs

static FreeCamera camera;

static bool left_mouse_button_pressed = false;
static bool right_mouse_button_pressed = false;


// Scenes

std::vector<Scene> scenes;
unsigned int selected_scene;

std::vector<std::pair<kmath::Vec3, kmath::Vec3>> rays;


// =========================
// = Some helper functions =
// =========================


long get_time_millis() {
  static auto prog_start = std::chrono::system_clock::now();
  
  auto now = std::chrono::system_clock::now();
  auto dur = now - prog_start;
  auto millis_dur = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
  long millis = millis_dur.count();
  return millis;
}


float get_time_delta(const long p_from, const long p_to) {
  return (float)(p_to - p_from) / 1000.0f;
}


void load_perspective_projection_matrix(const float aspect_ratio) {
  glMatrixMode(GL_PROJECTION);
  kmath::Mat4 projection = kmath::Mat4::perspective_lh_no_ndc_vfov(
    0.1f,
    20.0f,
    kmath::PI * 75.0 / 180.0,
    aspect_ratio
  );
  load_matrix(projection);
  glMatrixMode(GL_MODELVIEW);
}


void load_orthogonal_projection_matrix(const float width, const float height) {
  glMatrixMode(GL_PROJECTION);
  kmath::Mat4 projection = kmath::Mat4::orthogonal_lh_no_ndc(
    0.1f,
    10.0f,
    width,
    height
  );
  load_matrix(projection);
  glMatrixMode(GL_MODELVIEW);
}


void toggle_fullscreen() {
  GLFWmonitor *monitor = glfwGetWindowMonitor(window);

  if (fullscreen) {
    glfwSetWindowMonitor(window, nullptr, 0, 0, window_width, window_height, 0);
  } else {
    const GLFWvidmode *view_mode = glfwGetVideoMode(monitor);
    glfwSetWindowMonitor(window, monitor, 0, 0, view_mode->width, view_mode->height, view_mode->refreshRate);
  }

  fullscreen = !fullscreen;
}


kmath::Vec2 get_window_center() {
  return 0.5f * kmath::Vec2(window_width, window_height);
}


void center_mouse() {
  const kmath::Vec2 center = get_window_center();
  glfwSetCursorPos(window, center.x, center.y);
}


// =====================
// = Drawing functions =
// =====================


void draw() {
  glMatrixMode(GL_MODELVIEW_MATRIX);
  load_matrix(camera.get_view_matrix());

  // Draw scene
  glEnable(GL_LIGHTING);
  scenes[selected_scene].draw();

  // Draw rays
  // std::cout << rays.size() << std::endl;
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glLineWidth(6);
  glColor3f(1,0,0);
  glBegin(GL_LINES);
  for (unsigned int r = 0 ; r < rays.size() ; ++r) {
    glVertex3f( rays[r].first[0],rays[r].first[1],rays[r].first[2] );
    glVertex3f( rays[r].second[0], rays[r].second[1], rays[r].second[2] );
  }
  glEnd();
}


void ray_trace_from_camera() {
  using namespace kmath;
  
  const size_t image_width = window_width;
  const size_t image_height = window_height;
  std::cout << "Ray tracing a " << image_width << " x " << image_height << " image" << std::endl;

  // unsigned int nsamples = 100;
  const unsigned int sample_count = 50;
  std::vector<Vec3> image(image_width * image_height, Vec3::ZERO);

  const Mat4 inv_proj = inverse(get_projection_matrixf());
  const Mat4 inv_view = camera.get_inv_view_matrix();
  const Mat4 inv_mv = inv_view * inv_proj;

  const Vec3 camera_position = homogeneous_projection(inv_view * Vec4(Vec3::ZERO, 1.0));
  const float near_plane = get_depth_range().x;

  // Create image
  for (size_t y = 0; y < image_height; y++){
    float v = ((float)y + (float)(rand()) / (float)(RAND_MAX)) / image_height;
    for (size_t x = 0; x < image_width; x++) {
      for(unsigned int s = 0 ; s < sample_count ; ++s) {
        float u = ((float)x + (float)(rand()) / (float)(RAND_MAX)) / image_width;
        const Vec3 ray_direction = homogeneous_projection(inv_mv * Vec4(2.0f * u - 1.0f, 2.0f * v - 1.0f, -near_plane, 1.0)) - camera_position;
        
        const Vec3 color = scenes[selected_scene].rayTrace(Ray(camera_position, ray_direction));
        image[x + y * image_width] += color;
      }
      image[x + y * image_width] /= static_cast<float>(sample_count);
    }
  }

  std::cout << "\tDone" << std::endl;

  // Write the raytraced image to a file
  std::string filename = "./rendu.ppm";
  std::ofstream f(filename.c_str(), std::ios::binary);
  if (f.fail()) {
    std::cout << "Could not open file: " << filename << std::endl;
    return;
  }
  f << "P3" << std::endl << image_width << " " << image_height << std::endl << 255 << std::endl;
  for (size_t i = 0; i < image_width * image_height; i++)
    f << (int)(255.f*std::min<float>(1.f,image[i].x)) << " " << (int)(255.f*std::min<float>(1.f,image[i].y)) << " " << (int)(255.f*std::min<float>(1.f,image[i].z)) << " ";
  f << std::endl;
  f.close();
}


// ===================
// = Update function =
// ===================



void update() {
  // Display frame delta
  static long fps_prev_measure_time = get_time_millis();
  static long fps_prev_measure_frame_count = frame_count;

  const long current_time = get_time_millis();
  const float fps_delta = get_time_delta(current_time, fps_prev_measure_time);
  
  if (fps_delta >= 1000.0f) {
    const long frame_count_delta = frame_count - fps_prev_measure_frame_count;
    frame_rate = fps_delta / frame_count_delta;

    static char title[64];
    sprintf(title, "Raytracer - FPS: %d", frame_rate);
    glfwSetWindowTitle(window, title);

    fps_prev_measure_frame_count = frame_count;
    fps_prev_measure_time = current_time;
  }

  // Handle movement
  kmath::Vec3 movement_dir;
  
  if (glfwGetKey(window, GLFW_KEY_W)) {
    movement_dir.z += 1.0;
  }
  if (glfwGetKey(window, GLFW_KEY_A)) {
    movement_dir.x -= 1.0;
  }
  if (glfwGetKey(window, GLFW_KEY_S)) {
    movement_dir.z -= 1.0;
  }
  if (glfwGetKey(window, GLFW_KEY_D)) {
    movement_dir.x += 1.0;
  }
  if (glfwGetKey(window, GLFW_KEY_Q)) {
    movement_dir.y -= 1.0;
  }
  if (glfwGetKey(window, GLFW_KEY_E)) {
    movement_dir.y += 1.0;
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
    movement_dir *= 2.5f;
  }

  camera.local_translate(movement_dir * frame_delta);
}


// ==================
// = Input handling =
// ==================


void key_callback([[maybe_unused]] GLFWwindow *p_window, int p_key, [[maybe_unused]] int p_scancode, int p_action, [[maybe_unused]] int p_modifiers) {
  if (p_action != GLFW_PRESS) return;
  
  switch (p_key) {
  case GLFW_KEY_F:
    toggle_fullscreen();
    break;

  case GLFW_KEY_ESCAPE:
    exit(EXIT_SUCCESS);
    break;

  case GLFW_KEY_Z:
    GLint polygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, polygonMode);
    if(polygonMode[0] != GL_FILL)
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;

  case GLFW_KEY_R:
    rays.clear();
    ray_trace_from_camera();
    break;

  case GLFW_KEY_N:
    selected_scene++;
    if (selected_scene >= scenes.size()) selected_scene = 0;
    break;

  default:
    // FIXME: add usage print ?
    // printUsage();
    break;
  }
}


void mouse_button([[maybe_unused]] GLFWwindow *p_window, int p_button, int p_action, [[maybe_unused]] int p_mods) {
  switch (p_button) {
  case GLFW_MOUSE_BUTTON_LEFT:
    left_mouse_button_pressed = (p_action == GLFW_PRESS);
    break;
  case GLFW_MOUSE_BUTTON_RIGHT:
    right_mouse_button_pressed = (p_action == GLFW_PRESS);
    if (right_mouse_button_pressed) {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    } else {
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    center_mouse();
    break;
  }
}


void mouse_motion([[maybe_unused]] GLFWwindow *p_window, double p_xpos, double p_ypos) {
  if (!right_mouse_button_pressed) return;
  
  const kmath::Vec2 current_mouse_pos = kmath::Vec2(p_xpos, p_ypos);
  const kmath::Vec2 mouse_delta = current_mouse_pos - get_window_center();
  const kmath::Vec2 rotation_delta = 0.5f * frame_delta * mouse_delta;

  camera.local_rotate(rotation_delta);

  center_mouse();
}


void resized([[maybe_unused]] GLFWwindow *p_window, int width, int height) {
  window_width = width;
  window_height = height;
  load_perspective_projection_matrix((float)window_width / window_height);
  glViewport(0, 0, window_width, window_height);
}


// ========
// = Help =
// ========


void print_help() {
  // TODO: change :)
  std::cerr << std::endl
    << "gMini: a minimal OpenGL/GLUT application" << std::endl
    << "for 3D graphics." << std::endl
    << "Author : Tamy Boubekeur (http://www.labri.fr/~boubek)" << std::endl << std::endl
    << "Usage : ./gmini [<file.off>]" << std::endl
    << "Keyboard commands" << std::endl
    << "------------------" << std::endl
    << " ?: Print help" << std::endl
    << " w: Toggle Wireframe Mode" << std::endl
    << " g: Toggle Gouraud Shading Mode" << std::endl
    << " f: Toggle full screen mode" << std::endl
    << " <drag>+<left button>: rotate model" << std::endl
    << " <drag>+<right button>: move model" << std::endl
    << " <drag>+<middle button>: zoom" << std::endl
    << " q, <esc>: Quit" << std::endl << std::endl;
}


// =======================
// = Initialization code =
// =======================


void init_light() {
  GLfloat light_position[4] = {0.0, 1.5, 0.0, 1.0};
  GLfloat color[4] = { 1.0, 1.0, 1.0, 1.0};
  GLfloat ambient[4] = { 1.0, 1.0, 1.0, 1.0};

  glLightfv(GL_LIGHT1, GL_POSITION, light_position);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, color);
  glLightfv(GL_LIGHT1, GL_SPECULAR, color);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHTING);
}


void init_scenes() {
  camera.position = kmath::Vec3(0., 0., 3.1);

  selected_scene = 0;
  scenes.resize(3);
  scenes[0].setup_single_sphere();
  scenes[1].setup_single_square();
  scenes[2].setup_cornell_box();

  init_light();
  resized(window, window_width, window_height);
}


GLFWwindow *init_window() {
  if (!glfwInit()) return nullptr;
  
  GLFWwindow *window = glfwCreateWindow(
    window_width, window_height,
    "TP 2",
    NULL,
    NULL
  );
  if (!window) {
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, &key_callback);

  glFrontFace(GL_CCW);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);

  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);

  glClearColor(0.08f, 0.02f, 0.12f, 1.0f);

  // glEnable(GL_COLOR_MATERIAL);
  glPointSize(8.0f);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

  glEnable(GL_DEBUG_OUTPUT);

  return window;
}


int main() {
  print_help();

  // Init
  window = init_window();

  glfwSetKeyCallback(window, &key_callback);
  glfwSetWindowSizeCallback(window, &resized);
  glfwSetMouseButtonCallback(window, &mouse_button);
  glfwSetCursorPosCallback(window, &mouse_motion);

  init_scenes();

  // Main loop
  prev_frame_time = get_time_millis();

  while (!glfwWindowShouldClose(window)) {
    long current_time = get_time_millis();
    frame_delta = get_time_delta(prev_frame_time, current_time);
    prev_frame_time = current_time;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    update();
    draw();

    glFlush();
    glfwSwapBuffers(window);
    glfwPollEvents();

    frame_count += 1;
  }
  
  return EXIT_SUCCESS;
}

