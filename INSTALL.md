# PCB OpenViewer - Installation Guide

This guide helps you install and set up the PCB OpenViewer on your system.

## Prerequisites

### Windows

**Option 1: Using Visual Studio (Recommended)**
1. Install [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) with:
   - Desktop development with C++
   - CMake tools for C++
   - Git for Windows

2. Install [vcpkg](https://github.com/Microsoft/vcpkg) for dependency management:
   ```cmd
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install
   ```
   
3. Set environment variable `VCPKG_ROOT` to your vcpkg installation directory

**Option 2: Manual Dependencies**
1. Install [CMake](https://cmake.org/download/)
2. Install [GLFW3](https://www.glfw.org/download.html) development libraries
3. Install [GLEW](http://glew.sourceforge.net/) development libraries
4. Ensure OpenGL drivers are up to date

### Linux (Ubuntu/Debian)

```bash
# Install build tools
sudo apt-get update
sudo apt-get install build-essential cmake git

# Install OpenGL and windowing libraries
sudo apt-get install libgl1-mesa-dev libglfw3-dev libglew-dev

# Optional: Install pkg-config for easier dependency detection
sudo apt-get install pkg-config
```

### Linux (CentOS/RHEL/Fedora)

```bash
# Install build tools
sudo yum install gcc-c++ cmake git
# or for newer versions: sudo dnf install gcc-c++ cmake git

# Install OpenGL and windowing libraries
sudo yum install mesa-libGL-devel glfw-devel glew-devel
# or for newer versions: sudo dnf install mesa-libGL-devel glfw-devel glew-devel
```

### macOS

```bash
# Install Xcode command line tools
xcode-select --install

# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake glfw glew
```

## Building from Source

### Windows with vcpkg

1. Open Command Prompt or PowerShell
2. Navigate to the openviewer directory
3. Run the build script:
   ```cmd
   build_windows_vcpkg.bat
   ```

### Windows without vcpkg

1. Ensure all dependencies are installed and in your PATH
2. Open Command Prompt or PowerShell
3. Navigate to the openviewer directory
4. Run the build script:
   ```cmd
   build_windows.bat
   ```

### Linux/macOS

1. Open terminal
2. Navigate to the openviewer directory
3. Make the build script executable and run it:
   ```bash
   chmod +x build_unix.sh
   ./build_unix.sh
   ```

### Manual Build

If the build scripts don't work, you can build manually:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

## Running the Application

### With XZZPCB Files

```bash
# Windows
build\Release\pcb_viewer.exe path\to\your\file.xzzpcb

# Linux/macOS
./build/pcb_viewer path/to/your/file.xzzpcb
```

### Demo Mode (No Files)

```bash
# Windows
build\Release\pcb_viewer.exe

# Linux/macOS
./build/pcb_viewer
```

The application will generate sample PCB data for demonstration.

## Troubleshooting

### Common Build Issues

**CMake not found**
- Ensure CMake is installed and in your PATH
- On Windows, restart your command prompt after installing CMake

**OpenGL libraries not found**
- Update your graphics drivers
- On Linux, ensure mesa development packages are installed
- On Windows, ensure you have Visual Studio with C++ development tools

**GLFW/GLEW not found**
- On Windows with vcpkg: Ensure VCPKG_ROOT is set correctly
- On Linux: Install development packages with your package manager
- On macOS: Install dependencies with Homebrew

**Visual Studio errors (Windows)**
- Ensure you have "Desktop development with C++" workload installed
- Try running the build from "Developer Command Prompt for VS 2022"

### Runtime Issues

**Application crashes on startup**
- Check if your graphics drivers support OpenGL 3.3 or higher
- Try updating your graphics drivers
- On Linux, ensure you're not running in a virtual machine without 3D acceleration

**File loading errors**
- Ensure the XZZPCB file is not corrupted
- Check file permissions
- Try the demo mode first to verify the application works

**Performance issues**
- Close other GPU-intensive applications
- Try reducing window size if possible
- Check if hardware acceleration is enabled

## Getting Help

If you encounter issues:

1. Check the troubleshooting section above
2. Look at the console output for error messages
3. Try building in debug mode for more detailed error information:
   ```bash
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   ```
4. Create an issue on the project repository with:
   - Your operating system and version
   - Complete error messages
   - Steps to reproduce the problem

## System Requirements

### Minimum Requirements
- **OS**: Windows 10, Ubuntu 18.04, macOS 10.14, or equivalent
- **Graphics**: OpenGL 3.3 compatible graphics card
- **RAM**: 512 MB available memory
- **Storage**: 50 MB free disk space

### Recommended Requirements
- **OS**: Windows 11, Ubuntu 20.04+, macOS 11+
- **Graphics**: Dedicated graphics card with OpenGL 4.0+ support
- **RAM**: 2 GB available memory
- **Storage**: 200 MB free disk space (for build tools and cache)

## Next Steps

After successful installation:

1. Read the main README.md for usage instructions
2. Try loading some XZZPCB files
3. Explore the keyboard and mouse controls
4. Check the DEVELOPMENT.md if you want to contribute to the project
