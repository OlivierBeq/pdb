PROJECT(pdbget CXX)
cmake_minimum_required(VERSION 3.0)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_EPORTS_COMPILE_COMMANDS ON)
set(CMAKE_SOURCE_DIR "./src/")

if(NOT BUILD_REVISION)
  set(BUILD_REVISION 0 CACHE STRING "")
else()
  math(EXPR BUILD_REVISION "${BUILD_REVISION} + 1")
endif()

add_definitions(-DBUILD_REVISION=${BUILD_REVISION})

project(sample)

add_executable(
  pdbget
  Main.cpp
  StringMagic.cpp
  HTTPDownloader.cpp)

target_link_libraries(
  pdbget
  curl)

