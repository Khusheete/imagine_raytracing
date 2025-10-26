cmake_minimum_required(VERSION 3.22)

project(raytracing VERSION 1.0)


# == Compilation options ==

option(DEBUG_BUILD "Build project with debug symbols" ON)


# == Project options ==
set(CMAKE_EXPORT_COMPILE_COMMANDS True)


# == Build options ==
add_library(build_options INTERFACE)
target_compile_features(build_options INTERFACE
  cxx_std_20
)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")
if(NOT gcc_like_cxx)
  message(FATAL_ERROR "Cannot compile project with a compiler other than clang and gcc")
endif()

# Add all the warnings :)
target_compile_options(build_options INTERFACE
  -Wall -Wextra
)
# Optimizations
target_compile_options(build_options INTERFACE
  -O3
)

if(DEBUG_BUILD)
  target_compile_options(build_options INTERFACE
    -ggdb
  )
else()
  target_compile_definitions(build_options INTERFACE "NDEBUG")
endif()


# == Build libraries ==
add_subdirectory(kmath/)


# == Build and configure libs ==
add_executable(raytracing
  src/main.cpp
  src/scene.cpp
  src/imageLoader.cpp
  # src/material.cpp
  # src/light.cpp

  src/geometry/ray.cpp
  src/geometry/plane.cpp
  src/geometry/mesh.cpp
  src/geometry/sphere.cpp
  src/geometry/square.cpp

  src/utils/gl_utils.cpp
  src/utils/drawing_primitives.cpp
  src/utils/camera.cpp
)

target_include_directories(raytracing PRIVATE
  "${PROJECT_SOURCE_DIR}" src/
)

target_link_libraries(raytracing PUBLIC
  build_options
  kmath
  glfw GL m pthread
)
