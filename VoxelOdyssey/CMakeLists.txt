cmake_minimum_required(VERSION 3.20)
project(VoxelOdyssey)

set(CMAKE_CXX_STANDARD 17)

#Find packages via vcpkg
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(stb CONFIG REQUIRED)

#include dirs
include_directories(src)

#Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

#Executable
add_executable(VoxelOdyssey ${SOURCES})

#link libs
target_link_libraries(VoxelOdyssey PRIVATE glfw glm::glm glad::glad stb::stb)

#copy assets to build dir(later)