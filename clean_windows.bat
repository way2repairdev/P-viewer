@echo off
echo Cleaning PCB OpenViewer build artifacts...
echo.

if exist build (
    echo Removing build directory...
    rmdir /s /q build
    echo Build directory removed.
) else (
    echo Build directory does not exist.
)

if exist *.vcxproj (
    echo Removing Visual Studio project files...
    del /q *.vcxproj
    del /q *.vcxproj.filters
    del /q *.vcxproj.user
    del /q *.sln
)

if exist CMakeCache.txt (
    echo Removing CMake cache files...
    del /q CMakeCache.txt
    del /q cmake_install.cmake
    del /q CPackConfig.cmake
    del /q CPackSourceConfig.cmake
)

if exist CMakeFiles (
    echo Removing CMakeFiles directory...
    rmdir /s /q CMakeFiles
)

echo.
echo Clean completed!
echo You can now run the build script to create a fresh build.
pause
