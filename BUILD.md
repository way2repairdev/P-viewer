# Build System Documentation

## Overview
The PCB Viewer project uses CMake for cross-platform building and vcpkg for dependency management. The build system is designed to be portable and work on any system without hardcoded paths.

## Prerequisites

### Windows
- Visual Studio 2019 or later with C++ tools
- CMake 3.15 or later
- Git

### Linux
- GCC or Clang compiler
- CMake 3.15 or later
- Development packages for OpenGL, X11

### macOS
- Xcode command line tools
- CMake 3.15 or later

## Dependencies
All dependencies are managed through vcpkg and should be installed in the `vcpkg_installed/` directory:
- GLFW (window management)
- GLEW (OpenGL extension loading)
- ImGui (UI framework)
- OpenGL (graphics)

## Building

### Quick Start (Recommended)
Use the provided build scripts for a clean build:

**Windows:**
```batch
build_clean.bat
```

**Linux/macOS:**
```bash
chmod +x build_clean.sh
./build_clean.sh
```

### Manual Build
```bash
# Clean previous build
rm -rf build     # Linux/macOS
# or
rmdir /s /q build  # Windows

# Create and enter build directory
mkdir build
cd build

# Configure
cmake ..

# Build
cmake --build . --config Release
```

## Project Structure
```
P-viewer/
├── src/                    # Source code
│   ├── core/              # Core functionality
│   ├── formats/           # File format handlers
│   ├── renderer/          # Rendering engine
│   └── main.cpp           # Entry point
├── test_files/            # Sample PCB files
├── vcpkg_installed/       # Dependencies (auto-generated)
├── build/                 # Build directory (excluded from git)
├── CMakeLists.txt         # Build configuration
├── vcpkg.json             # Dependency manifest
└── .gitignore             # Git ignore rules
```

## Portability Features
- All paths in CMakeLists.txt are relative to the project directory
- Platform-specific library linking (Windows/Linux/macOS)
- Automatic vcpkg triplet detection
- Build artifacts excluded from version control
- Clean build scripts provided

## Troubleshooting

### Build Fails with Missing Libraries
1. Ensure vcpkg dependencies are installed in `vcpkg_installed/[triplet]/`
2. Try a clean build: delete the `build/` directory and rebuild

### Path Issues
1. The build system uses only relative paths
2. Generated files (CMakeCache.txt, .vcxproj) contain absolute paths but are excluded from git
3. Use the clean build scripts to avoid cached path issues

### Platform-Specific Issues
- **Windows**: Ensure Visual Studio C++ tools are installed
- **Linux**: Install development packages: `sudo apt-get install libgl1-mesa-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev`
- **macOS**: Install Xcode command line tools: `xcode-select --install`

## Generated Files (Excluded from Git)
The following files/directories are automatically generated and excluded from version control:
- `build/` and `build-debug/` directories
- `CMakeCache.txt`
- `*.vcxproj` and `*.sln` files
- Object files and executables
- IDE-specific files (.vs/, .vscode/)
