@echo off
echo Building PCB OpenViewer with vcpkg dependencies...
echo.

REM Check if vcpkg is available
if not exist "%VCPKG_ROOT%" (
    echo ERROR: VCPKG_ROOT environment variable is not set
    echo Please install vcpkg and set VCPKG_ROOT to the vcpkg installation directory
    echo See: https://github.com/Microsoft/vcpkg
    pause
    exit /b 1
)

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

REM Configure project with vcpkg toolchain
echo Configuring project with vcpkg dependencies...
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed
    echo Make sure you have Visual Studio 2022 installed and vcpkg is properly set up
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
