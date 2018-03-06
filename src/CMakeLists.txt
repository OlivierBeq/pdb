cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_EPORTS_COMPILE_COMMANDS ON)

project(sample)

add_executable(
  pdb
  Main.cpp
  HTTPDownloader.cpp)

target_link_libraries(
  pdb
  curl)

