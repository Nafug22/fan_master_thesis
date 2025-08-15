cmake_minimum_required(VERSION 3.22)
project(FineVGPU LANGUAGES CUDA CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SERVER ON)

set(BUILD_DIR ${CMAKE_BINARY_DIR} CACHE INTERNAL "build_directory")
set(CUDA_HOME /usr/local/cuda)
include_directories(${CUDA_HOME}/include)

find_library(CUDA_DRIVER_LIB cuda PATHS ${CUDA_HOME}/include)
if(!CUDA_DRIVER_LIB)
    message(FATAL_ERROR "CUDA Driver Library not found")
endif()

enable_testing()
add_subdirectory(interception)
add_subdirectory(tests)