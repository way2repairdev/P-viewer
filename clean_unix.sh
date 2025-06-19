#!/bin/bash

echo "Cleaning PCB OpenViewer build artifacts..."
echo

if [ -d "build" ]; then
    echo "Removing build directory..."
    rm -rf build
    echo "Build directory removed."
else
    echo "Build directory does not exist."
fi

# Remove other common build artifacts
if [ -f "CMakeCache.txt" ]; then
    echo "Removing CMake cache files..."
    rm -f CMakeCache.txt
    rm -f cmake_install.cmake
    rm -f CPackConfig.cmake
    rm -f CPackSourceConfig.cmake
fi

if [ -d "CMakeFiles" ]; then
    echo "Removing CMakeFiles directory..."
    rm -rf CMakeFiles
fi

# Remove compiled objects and other artifacts
find . -name "*.o" -delete 2>/dev/null
find . -name "*.a" -delete 2>/dev/null
find . -name "*.so" -delete 2>/dev/null
find . -name "*.dylib" -delete 2>/dev/null

echo
echo "Clean completed!"
echo "You can now run ./build_unix.sh to create a fresh build."
