#!/bin/bash
echo "Cleaning build directory..."
rm -rf build
mkdir build
cd build

echo "Configuring project..."
cmake ..

echo "Building project..."
cmake --build . --config Release

echo "Build complete! The executable is in build/pcb_viewer"
