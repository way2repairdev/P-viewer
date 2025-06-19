@echo off
echo Building PCB OpenViewer for Windows...
echo.

REM Check if CMake is available
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: CMake is not installed or not in PATH
    echo Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
)

REM Create build directory
if not exist build mkdir build
cd build

REM Configure project
echo Configuring project with Visual Studio 2022...
cmake .. -G "Visual Studio 17 2022" -A x64
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed
    echo Make sure you have Visual Studio 2022 installed with C++ development tools
    echo Also ensure you have the required dependencies:
    echo - OpenGL development libraries
    echo - GLFW3 development libraries
    echo - GLEW development libraries
    pause
    exit /b 1
)

REM Build project
echo Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo Executable location: build\Release\pcb_viewer.exe
echo.
pause
