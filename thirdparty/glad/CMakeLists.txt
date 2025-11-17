add_library(glad STATIC
  src/glad.c
)

target_compile_features(glad PRIVATE cxx_std_11)
target_compile_options(glad PRIVATE -O3)
target_include_directories(glad PRIVATE include/)
