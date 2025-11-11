#include <GL/gl.h>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <ostream>
#include <random>
#include <thread>
#include <vector>
#include <string>
#include <chrono>

#include <GLFW/glfw3.h>

#include "kmath/constants.hpp"
#include "kmath/vector.hpp"
#include "utils/camera.hpp"
#include "scene.h"
#include "utils/drawing_primitives.hpp"
#include "utils/gl_utils.hpp"
#include "utils/image.hpp"
#include "utils/profiler.hpp"
#include "utils/thread_group.hpp"


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

std::vector<std::pair<Ray, kmath::Vec2>> rays;


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
  glBegin(GL_LINES);
  for (unsigned int r = 0 ; r < rays.size() ; ++r) {
    const kmath::Vec3 point = rays[r].first.origin;
    const kmath::Vec3 dir = rays[r].first.direction;
    const kmath::Vec3 tip = point + dir;
    const kmath::Vec2 uv = rays[r].second;
    glColor3f(uv.x, uv.y, 0.0);
    glVertex3f(point.x, point.y, point.z);
    glVertex3f(tip.x, tip.y, tip.z);
  }
  glEnd();
}

// Polynomial approximation of EaryChow's AgX sigmoid curve.
// x must be within the range [0.0, 1.0]
kmath::Vec3 agx_contrast_approx(kmath::Vec3 x) {
	// Generated with Excel trendline
	// Input data: Generated using python sigmoid with EaryChow's configuration and 57 steps
	// Additional padding values were added to give correct intersections at 0.0 and 1.0
	// 6th order, intercept of 0.0 to remove an operation and ensure intersection at 0.0
	kmath::Vec3 x2 = x * x;
	kmath::Vec3 x4 = x2 * x2;
	return 0.021f * x + 4.0111f * x2 - 25.682f * x2 * x + 70.359f * x4 - 74.778f * x4 * x + 27.069f * x4 * x2;
}

// This code is adapted from the Godot game engine
// This is an approximation and simplification of EaryChow's AgX implementation that is used by Blender.
// This code is based off of the script that generates the AgX_Base_sRGB.cube LUT that Blender uses.
// Source: https://github.com/EaryChow/AgX_LUT_Gen/blob/main/AgXBasesRGB.py
kmath::Vec3 tonemap_agx(kmath::Vec3 color) {
	// Combined linear sRGB to linear Rec 2020 and Blender AgX inset matrices:
	const kmath::Mat3 srgb_to_rec2020_agx_inset_matrix = kmath::Mat3(
    kmath::Vec3(0.54490813676363087053, 0.14044005884001287035, 0.088827411851915368603),
    kmath::Vec3(0.37377945959812267119, 0.75410959864013760045, 0.17887712465043811023),
    kmath::Vec3(0.081384976686407536266, 0.10543358536857773485, 0.73224999956948382528)
	);

	// Combined inverse AgX outset matrix and linear Rec 2020 to linear sRGB matrices.
	const kmath::Mat3 agx_outset_rec2020_to_srgb_matrix = kmath::Mat3(
    kmath::Vec3(1.9645509602733325934, -0.29932243390911083839, -0.16436833806080403409),
    kmath::Vec3(-0.85585845117807513559, 1.3264510741502356555, -0.23822464068860595117),
    kmath::Vec3(-0.10886710826831608324, -0.027084020983874825605, 1.402665347143271889)
  );

	// LOG2_MIN      = -10.0
	// LOG2_MAX      =  +6.5
	// MIDDLE_GRAY   =  0.18
	const float min_ev = -12.4739311883324;
	const float max_ev = 4.02606881166759;

	// Large negative values in one channel and large positive values in other
	// channels can result in a colour that appears darker and more saturated than
	// desired after passing it through the inset matrix. For this reason, it is
	// best to prevent negative input values.
	// This is done before the Rec. 2020 transform to allow the Rec. 2020
	// transform to be combined with the AgX inset matrix. This results in a loss
	// of color information that could be correctly interpreted within the
	// Rec. 2020 color space as positive RGB values, but it is less common for Godot
	// to provide this function with negative sRGB values and therefore not worth
	// the performance cost of an additional matrix multiplication.
	// A value of 2e-10 intentionally introduces insignificant error to prevent
	// log2(0.0) after the inset matrix is applied; color will be >= 1e-10 after
	// the matrix transform.
	color = kmath::max(color, 2e-10f * kmath::Vec3::ONE);

	// Do AGX in rec2020 to match Blender and then apply inset matrix.
	color = srgb_to_rec2020_agx_inset_matrix * color;

	// Log2 space encoding.
	// Must be clamped because agx_contrast_approx may not work
	// well with values outside of the range [0.0, 1.0]
	color = kmath::apply(color, [](const float x) -> float { return std::log2(x); });
	color = kmath::apply(color, [&](const float x) -> float { return std::clamp(x, min_ev, max_ev); });
	color = (color - min_ev * kmath::Vec3::ONE) / (max_ev - min_ev);

	// Apply sigmoid function approximation.
	color = agx_contrast_approx(color);

	// Convert back to linear before applying outset matrix.
	color = kmath::apply(color, [](const float x) -> float { return pow(x, 2.4f); });

	// Apply outset to make the result more chroma-laden and then go back to linear sRGB.
	color = agx_outset_rec2020_to_srgb_matrix * color;

	return color;
}


void ray_trace_from_camera() {
  using namespace kmath;
  
  const size_t image_width = window_width;
  const size_t image_height = window_height;
  const float inv_image_width = 1.0f / static_cast<float>(image_width);
  const float inv_image_height = 1.0f / static_cast<float>(image_height);

  const unsigned int sample_count = 50;
  const float sample_division = 1.0f / static_cast<float>(sample_count);
  Image image(image_width, image_height);

  const Mat4 inv_proj = inverse(get_projection_matrixf());
  const Mat4 inv_view = camera.get_inv_view_matrix();
  const Mat4 inv_mvp = inv_view * inv_proj;

  const Vec3 camera_position = homogeneous_projection(inv_view * Vec4(Vec3::ZERO, 1.0));
  const float near_plane = get_depth_range().x;

  const uint32_t random_seed = 47;

  // Reset debug rays
  rays.clear();

  Profiler full_render_profile;
  full_render_profile.start();
  {
    Profiler specific_profiler;

    // Create thread work group to do work
    const size_t available_thread_count = std::thread::hardware_concurrency();
    size_t thread_count = (available_thread_count)? available_thread_count : 8;
    std::cout << "Ray tracing a " << image_width << " x " << image_height << " image on " << thread_count << " threads" << std::endl;
    ThreadWorkGroup work_group(thread_count);

    // Lambda to declare a rendering pass
    auto exec_phase = [&](const char *p_phase_name, const ParallelFunction &p_func) -> void {
      specific_profiler.start();

      // Start work
      work_group.execute(p_func, 0, image.get_size());

      // Report progress
      while (!work_group.is_work_done()) {
        const double progress = work_group.get_progress();
        const size_t ticks = progress * 40;
        std::cout << "\r" << p_phase_name << ": <";
        for (size_t i = 0; i < ticks; i++) {
          std::cout << "=";
        }
        for (size_t i = ticks; i < 40; i++) {
          std::cout << "-";
        }
        std::cout << "> " << std::setprecision(4) << progress * 100.0 << "%   ";
        std::flush(std::cout);
      
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1ms);
      }

      work_group.join();
      specific_profiler.end();

      std::cout << "\e[1M\r"; // Clear the line giving progress
      std::cout << "\t" << p_phase_name << " finished in " << specific_profiler.get_exec_time() << std::endl;
    };

    // Instantiate a random number generators for each thread
    std::vector<std::mt19937> rngs(thread_count);
    {
      std::mt19937 master_rng(random_seed);
      std::uniform_int_distribution<uint32_t> master_gen;
      
      for (size_t i = 0; i < thread_count; i++) {
        rngs[i] = std::mt19937(master_gen(master_rng));
      }
    }
    // TODO: use blue noise
    std::uniform_real_distribution<float> randf;

    // Execute render phases
    exec_phase(
      "Scene render",
      [&](const size_t p_thread_id, const size_t p_exec_index) -> void {
        std::mt19937 &rng = rngs[p_thread_id];
        const size_t x = p_exec_index % image_width;
        const size_t y = p_exec_index / image_width;

        for (unsigned int s = 0; s < sample_count; s++) {
          const float u = ((float)x + randf(rng)) * inv_image_width;
          const float v = ((float)y + randf(rng)) * inv_image_height;
          const Vec3 ray_direction = homogeneous_projection(inv_mvp * Vec4(2.0f * u - 1.0f, -2.0f * v + 1.0f, -near_plane, 1.0)) - camera_position;
          const Ray ray = Ray(camera_position, ray_direction);

          // const Vec3 color = scenes[selected_scene].ray_trace(ray);
          const Vec3 color = scenes[selected_scene].ray_trace_recursive(ray, 4);

          image(p_exec_index) += color;
        }

        image(p_exec_index) *= sample_division;
      }
    );

    exec_phase(
      "Tone mapping",
      [&]([[maybe_unused]] const size_t p_thread_id, const size_t p_exec_index) -> void {
        image(p_exec_index) = tonemap_agx(image(p_exec_index));
      }
    );
  }
  full_render_profile.end();
  std::cout << "\tRender done in " << full_render_profile.get_exec_time() << std::endl;

  // Write the raytraced image to a file
  std::string filename = "./rendu.ppm";
  std::ofstream f(filename.c_str(), std::ios::binary);
  if (f.fail()) {
    std::cout << "Could not open file: " << filename << std::endl;
    return;
  }
  image.write_ppm(f);
  f.close();

  std::cout << "Image saved." << std::endl;
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

