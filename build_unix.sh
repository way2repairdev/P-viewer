#!/bin/bash

echo "Building PCB OpenViewer for Linux/macOS..."
echo

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake is not installed or not in PATH"
    echo "Please install CMake:"
    echo "  Ubuntu/Debian: sudo apt-get install cmake"
    echo "  CentOS/RHEL: sudo yum install cmake"
    echo "  macOS: brew install cmake"
    exit 1
fi

# Check for required packages on Linux
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Checking for required development packages..."
    
    # Check for OpenGL dev packages
    if ! pkg-config --exists gl; then
        echo "WARNING: OpenGL development libraries not found"
        echo "Install with: sudo apt-get install libgl1-mesa-dev (Ubuntu/Debian)"
        echo "           or: sudo yum install mesa-libGL-devel (CentOS/RHEL)"
    fi
    
    # Check for GLFW
    if ! pkg-config --exists glfw3; then
        echo "WARNING: GLFW3 development libraries not found"
        echo "Install with: sudo apt-get install libglfw3-dev (Ubuntu/Debian)"
        echo "           or: sudo yum install glfw-devel (CentOS/RHEL)"
    fi
    
    # Check for GLEW
    if ! pkg-config --exists glew; then
        echo "WARNING: GLEW development libraries not found"
        echo "Install with: sudo apt-get install libglew-dev (Ubuntu/Debian)"
        echo "           or: sudo yum install glew-devel (CentOS/RHEL)"
    fi
fi

# Create build directory
mkdir -p build
cd build

# Configure project
echo "Configuring project..."
cmake .. -DCMAKE_BUILD_TYPE=Release
if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed"
    echo "Make sure you have the required dependencies installed"
    exit 1
fi

# Build project
echo "Building project..."
make -j$(nproc 2>/dev/null || echo 4)
if [ $? -ne 0 ]; then
    echo "ERROR: Build failed"
    exit 1
fi

echo
echo "Build completed successfully!"
echo "Executable location: build/pcb_viewer"
echo
