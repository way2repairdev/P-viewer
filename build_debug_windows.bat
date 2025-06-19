@echo off
echo Building PCB OpenViewer in Debug mode...
echo.

REM Check if CMake is available
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: CMake is not installed or not in PATH
    pause
    exit /b 1
)

REM Create build directory
if not exist build-debug mkdir build-debug
cd build-debug

REM Configure project for debug
echo Configuring project for Debug build...
if defined VCPKG_ROOT (
    cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake"
) else (
    cmake .. -G "Visual Studio 17 2022" -A x64
)

if %errorlevel% neq 0 (
    echo ERROR: CMake configuration failed
    pause
    exit /b 1
)

REM Build project in debug mode
echo Building project in Debug mode...
cmake --build . --config Debug
if %errorlevel% neq 0 (
    echo ERROR: Debug build failed
    pause
    exit /b 1
)

echo.
echo Debug build completed successfully!
echo Executable location: build-debug\Debug\pcb_viewer.exe
echo.
echo Debug build includes:
echo - Full debugging symbols
echo - Runtime checks enabled
echo - Detailed error messages
echo - Assertions enabled
echo.
pause
