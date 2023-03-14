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
  $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU,Intel>:
    -ftemplate-backtrace-limit=0
    -march=${SPACES_MARCH}
  >
  $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU,Intel>:
    -Rpass=loop-vectorize
    -Rpass-missed=loop-vectorize
    -Rpass-analysis=loop-vectorize
    -fsave-optimization-record
  >
)

add_executable(md_iteration_comparison md_iteration_comparison.cpp)
target_link_libraries(md_iteration_comparison PRIVATE spaces)

