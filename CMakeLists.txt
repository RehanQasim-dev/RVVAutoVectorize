cmake_minimum_required(VERSION 3.10)
project(RVVAutoVectorize)

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Build the pass as a shared library
add_library(RVVVectorize SHARED src/RVVVectorize.cpp)
target_link_libraries(RVVVectorize ${LLVM_LIBRARIES})
set_target_properties(RVVVectorize PROPERTIES PREFIX "") # Remove "lib" prefix

# Specify C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)