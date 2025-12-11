#!/bin/bash

# Create a build directory
mkdir -p build
cd build

# Generate build files with CMake
cmake ..

# Build the project
make
