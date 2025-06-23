@echo off
echo Cleaning build directory...
if exist build rmdir /s /q build
mkdir build
cd build

echo Configuring project...
cmake .. -G "Visual Studio 17 2022" -A x64

echo Building project...
cmake --build . --config Release

echo Build complete! The executable is in build/Release/pcb_viewer.exe
pause
