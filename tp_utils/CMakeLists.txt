add_library(tputils STATIC
  # == Windowing and inputs ==
  src/windowing.cpp
  src/inputs.cpp

  # == Rendering ==
  src/rendering/rendering.cpp
  src/rendering/camera.cpp
  src/rendering/mesh.cpp
  src/rendering/immediate_geometry.cpp

  src/rendering/primitives/vbuffer.cpp
  src/rendering/primitives/shader.cpp
  src/rendering/primitives/texture.cpp
  src/rendering/primitives/framebuffer.cpp

  # == 3D model loaders ==
  src/model_loaders/off_object.cpp
  src/model_loaders/wavefront_object.cpp

  # == Utilities ==
  src/time.cpp
  src/utils.cpp
  src/source_preprocessing.cpp
)


target_link_libraries(tputils PUBLIC
  build_options
  kmath
  stb
)

target_include_directories(tputils PRIVATE src/)
