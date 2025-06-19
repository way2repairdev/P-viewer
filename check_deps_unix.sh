#!/bin/bash

echo "Checking PCB OpenViewer build dependencies..."
echo

ERROR_COUNT=0
WARNING_COUNT=0

# Check CMake
if command -v cmake &> /dev/null; then
    echo "[OK] CMake is available"
    cmake --version | head -1
else
    echo "[ERROR] CMake is not installed"
    echo "  Ubuntu/Debian: sudo apt-get install cmake"
    echo "  CentOS/RHEL: sudo yum install cmake"
    echo "  macOS: brew install cmake"
    ((ERROR_COUNT++))
fi

# Check C++ compiler
if command -v g++ &> /dev/null; then
    echo "[OK] g++ compiler is available"
    g++ --version | head -1
elif command -v clang++ &> /dev/null; then
    echo "[OK] clang++ compiler is available"  
    clang++ --version | head -1
else
    echo "[ERROR] No C++ compiler found"
    echo "  Ubuntu/Debian: sudo apt-get install build-essential"
    echo "  CentOS/RHEL: sudo yum groupinstall 'Development Tools'"
    echo "  macOS: xcode-select --install"
    ((ERROR_COUNT++))
fi

# Check pkg-config (helpful for dependency detection)
if command -v pkg-config &> /dev/null; then
    echo "[OK] pkg-config is available"
else
    echo "[WARNING] pkg-config not found (recommended for dependency detection)"
    echo "  Ubuntu/Debian: sudo apt-get install pkg-config"
    echo "  CentOS/RHEL: sudo yum install pkgconfig"
    ((WARNING_COUNT++))
fi

# Check OpenGL development libraries
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    if pkg-config --exists gl 2>/dev/null; then
        echo "[OK] OpenGL development libraries found"
    else
        echo "[ERROR] OpenGL development libraries not found"
        echo "  Ubuntu/Debian: sudo apt-get install libgl1-mesa-dev"
        echo "  CentOS/RHEL: sudo yum install mesa-libGL-devel"
        ((ERROR_COUNT++))
    fi
    
    # Check GLFW
    if pkg-config --exists glfw3 2>/dev/null; then
        echo "[OK] GLFW3 development libraries found"
    else
        echo "[ERROR] GLFW3 development libraries not found"
        echo "  Ubuntu/Debian: sudo apt-get install libglfw3-dev"
        echo "  CentOS/RHEL: sudo yum install glfw-devel"
        ((ERROR_COUNT++))
    fi
    
    # Check GLEW
    if pkg-config --exists glew 2>/dev/null; then
        echo "[OK] GLEW development libraries found"
    else
        echo "[ERROR] GLEW development libraries not found"
        echo "  Ubuntu/Debian: sudo apt-get install libglew-dev"
        echo "  CentOS/RHEL: sudo yum install glew-devel"
        ((ERROR_COUNT++))
    fi
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS - check if Homebrew packages are installed
    if brew list glfw &> /dev/null; then
        echo "[OK] GLFW is installed via Homebrew"
    else
        echo "[WARNING] GLFW not found via Homebrew"
        echo "  Install with: brew install glfw"
        ((WARNING_COUNT++))
    fi
    
    if brew list glew &> /dev/null; then
        echo "[OK] GLEW is installed via Homebrew"
    else
        echo "[WARNING] GLEW not found via Homebrew"
        echo "  Install with: brew install glew"
        ((WARNING_COUNT++))
    fi
fi

# Check Git
if command -v git &> /dev/null; then
    echo "[OK] Git is available"
else
    echo "[WARNING] Git is not available"
    echo "  Ubuntu/Debian: sudo apt-get install git"
    echo "  CentOS/RHEL: sudo yum install git"
    echo "  macOS: xcode-select --install"
    ((WARNING_COUNT++))
fi

echo
if [ $ERROR_COUNT -eq 0 ]; then
    echo "All critical dependencies are available!"
    if [ $WARNING_COUNT -gt 0 ]; then
        echo "Found $WARNING_COUNT warning(s) - build may still work."
    fi
    echo "You should be able to build the project."
else
    echo "Found $ERROR_COUNT critical error(s)."
    echo "Please install the missing dependencies before building."
fi

echo
echo "Additional Information:"
echo "- For detailed installation instructions, see INSTALL.md"
echo "- For development guidance, see DEVELOPMENT.md"
echo "- Use ./build_unix.sh to build the project"
echo
