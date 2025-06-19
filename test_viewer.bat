@echo off
echo Testing PCB Viewer with File Open Dialog...
echo.
echo The PCB Viewer will start with these controls:
echo   - Left Mouse Button + Drag: Pan view
echo   - Mouse Wheel: Zoom in/out  
echo   - R Key: Reset view to fit PCB
echo   - Ctrl+O: Open PCB file dialog
echo   - ESC Key: Exit application
echo.
echo Starting PCB Viewer...
echo.

cd /d "%~dp0build\Release"
pcb_viewer.exe

echo.
echo PCB Viewer closed.
pause
