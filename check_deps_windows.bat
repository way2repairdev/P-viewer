@echo off
echo Checking PCB OpenViewer build dependencies...
echo.

set ERROR_COUNT=0

REM Check CMake
cmake --version >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] CMake is available
    cmake --version | findstr /C:"cmake version" | head -1
) else (
    echo [ERROR] CMake is not installed or not in PATH
    echo   Install from: https://cmake.org/download/
    set /a ERROR_COUNT+=1
)

REM Check Visual Studio
where cl.exe >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] Visual Studio C++ compiler is available
) else (
    echo [WARNING] Visual Studio C++ compiler not found in PATH
    echo   Make sure to run from "Developer Command Prompt for VS 2022"
    echo   Or install Visual Studio 2022 with C++ development tools
)

REM Check vcpkg
if defined VCPKG_ROOT (
    if exist "%VCPKG_ROOT%\vcpkg.exe" (
        echo [OK] vcpkg is available at: %VCPKG_ROOT%
    ) else (
        echo [WARNING] VCPKG_ROOT is set but vcpkg.exe not found
        echo   Current VCPKG_ROOT: %VCPKG_ROOT%
    )
) else (
    echo [INFO] VCPKG_ROOT not set - will use system-installed dependencies
)

REM Check Git
git --version >nul 2>&1
if %errorlevel% equ 0 (
    echo [OK] Git is available
) else (
    echo [WARNING] Git is not available
    echo   Install from: https://git-scm.com/download/win
)

echo.
if %ERROR_COUNT% equ 0 (
    echo All critical dependencies are available!
    echo You should be able to build the project.
) else (
    echo Found %ERROR_COUNT% critical error(s).
    echo Please install the missing dependencies before building.
)

echo.
echo Additional Information:
echo - For detailed installation instructions, see INSTALL.md
echo - For development guidance, see DEVELOPMENT.md
echo - Use build_windows_vcpkg.bat if you have vcpkg
echo - Use build_windows.bat if you have system dependencies
echo.
pause
