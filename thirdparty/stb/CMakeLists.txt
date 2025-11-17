add_library(stb STATIC
  stb_impl.cpp
)

target_compile_features(glad PRIVATE cxx_std_11)
target_compile_options(glad PRIVATE -O3)
target_include_directories(glad PRIVATE .)
