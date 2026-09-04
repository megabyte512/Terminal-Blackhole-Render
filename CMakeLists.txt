cmake_minimum_required(VERSION 3.28.3)
project(blackhole VERSION 1.0 LANGUAGES CXX)

set(SRC_FILES
  src/main.cpp
  src/input.cpp
  src/camera.cpp
  src/blackhole.cpp
  src/starfield.cpp
)

add_executable(blackhole ${SRC_FILES})
