cmake_minimum_required(VERSION 3.25)

project(spaces LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui.
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(SPACES_MARCH "native" CACHE STRING "Microarchitecture target (e.g. -march=)." FORCE)

add_library(spaces INTERFACE)

target_include_directories(spaces INTERFACE
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
                           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(spaces INTERFACE cxx_std_20)

target_compile_options(spaces INTERFACE
  $<$<AND:$<CONFIG:Release,RelWithDebInfo>,$<CXX_COMPILER_ID:Clang,AppleClang,GNU,Intel,NVHPC>>:
    -O3 -fstrict-aliasing
  >
  $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:Clang,AppleClang,GNU,Intel>>:
    -g0 -DNDEBUG
    -fno-exceptions -fno-rtti
    -fomit-frame-pointer -fno-asynchronous-unwind-tables -fno-unwind-tables
  >
  $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU,Intel,NVHPC>:
    -Wall
  >
  $<$<CXX_COMPILER_ID:Clang,AppleClang,Intel>:
    -Wno-pass-failed
    -Wno-assume
  >
  $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU,Intel>:
    -ftemplate-backtrace-limit=0
    -ferror-limit=0
    -march=${SPACES_MARCH}
  >
)

add_library(spaces_optimization_report INTERFACE)

target_compile_options(spaces_optimization_report INTERFACE
  $<$<CXX_COMPILER_ID:Clang,AppleClang,Intel>:
    -Rpass=loop-vectorize
    -Rpass-missed=loop-vectorize
    -Rpass-analysis=loop-vectorize
    -fsave-optimization-record
  >
)

add_executable(md_iteration_comparison md_iteration_comparison.cpp)
target_link_libraries(md_iteration_comparison PRIVATE spaces)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  add_library(md_iteration_comparison_optimization_report STATIC md_iteration_comparison.cpp)
  target_link_libraries(md_iteration_comparison_optimization_report PRIVATE spaces spaces_optimization_report)
  set(SPACES_THIS_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/md_iteration_comparison_optimization_report.dir)
  file(WRITE ${SPACES_THIS_BINARY_DIR}/redirect_compiler_output.bash
       "#! /usr/bin/env bash\n"
       "$@ > ${SPACES_THIS_BINARY_DIR}/compiler_output 2>&1\n")
  file(CHMOD ${SPACES_THIS_BINARY_DIR}/redirect_compiler_output.bash
       FILE_PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ)
  set_target_properties(md_iteration_comparison_optimization_report PROPERTIES
    CXX_COMPILER_LAUNCHER ${SPACES_THIS_BINARY_DIR}/redirect_compiler_output.bash)
endif()

