@echo off
echo Opening PCB Viewer with direct access to demo files...
echo.
echo Available demo files:
echo   - demo.xzzpcb (Primary test file)
echo   - demo.pcb (Alternative format)  
echo   - sample.xzzpcb (Simple test file)
echo.
echo Instructions:
echo   1. The viewer will start with sample PCB data
echo   2. Press Ctrl+O to open file dialog
echo   3. Select one of the demo files to test
echo   4. Use mouse to pan/zoom, R to reset view
echo.

cd /d "%~dp0build\Release"
start pcb_viewer.exe

echo.
echo File Explorer will open to show the demo files location...
start explorer "%~dp0test_files"

echo.
echo PCB Viewer started! Press Ctrl+O in the viewer to open files.
pause
