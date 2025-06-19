@echo off
echo ========================================
echo PCB Viewer - File Open Test
echo ========================================
echo.
echo Testing file open functionality...
echo.

set VIEWER_PATH=%~dp0build\Release\pcb_viewer.exe
set DEMO_FILE=%~dp0test_files\working_demo.xzzpcb

echo Viewer Location: %VIEWER_PATH%
echo Demo File: %DEMO_FILE%
echo.

if not exist "%VIEWER_PATH%" (
    echo ERROR: PCB Viewer executable not found!
    echo Make sure you have built the project first.
    pause
    exit /b 1
)

if not exist "%DEMO_FILE%" (
    echo ERROR: Demo file not found!
    echo Creating demo file...
    cd /d "%~dp0"
    python create_demo_xzzpcb.py
    if not exist "%DEMO_FILE%" (
        echo Failed to create demo file!
        pause
        exit /b 1
    )
)

echo.
echo Starting PCB Viewer with working demo file...
echo.
echo Expected behavior:
echo   1. Window opens showing sample PCB data
echo   2. Console shows file loading messages
echo   3. You can use Ctrl+O to test file dialog
echo   4. Mouse controls work for pan/zoom
echo.

cd /d "%~dp0build\Release"
echo Running: pcb_viewer.exe "%DEMO_FILE%"
echo.

start "" pcb_viewer.exe "%DEMO_FILE%"

echo.
echo PCB Viewer started! Check the console output for loading status.
echo.
echo Controls:
echo   - Left Mouse + Drag: Pan view
echo   - Mouse Wheel: Zoom in/out
echo   - R Key: Reset view
echo   - Ctrl+O: Open file dialog
echo   - ESC: Exit
echo.
pause
