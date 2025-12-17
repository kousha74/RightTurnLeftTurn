cmake_minimum_required(VERSION 3.10)

# Project name
project(HelloWorld)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable debug symbols and disable optimizations for better debugging
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")

# Add executable
add_executable(HelloWorld main.cpp TurnPuzzle.cpp Cell.cpp Edge.cpp Path.cpp)
