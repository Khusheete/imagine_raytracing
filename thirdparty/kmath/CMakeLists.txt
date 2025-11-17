cmake_minimum_required(VERSION 3.15)


# == Compilation options ==

option(DEBUG_BUILD "Build project with debug symbols" OFF)


# == Build options ==
add_library(kmath_build_options INTERFACE)
target_compile_features(kmath_build_options INTERFACE
  cxx_std_20
)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,Clang,GNU>")
if(gcc_like_cxx) # Options for gcc and clang
  target_compile_options(kmath_build_options INTERFACE
    -Wall -Wextra -O3
  )

  if(DEBUG_BUILD)
    target_compile_options(kmath_build_options INTERFACE
      -ggdb
    )
  else()
    target_compile_definitions(kmath_build_options INTERFACE "NDEBUG")
  endif()
endif()


# == Build library ==
add_library(kmath STATIC
  color.cpp
)

# target_include_directories(kmath PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(kmath PRIVATE kmath_build_options)
