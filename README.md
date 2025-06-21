# PCB Viewer

A professional PCB (Printed Circuit Board) viewer application with modern UI and advanced visualization features.

## Features

### Professional PCB Visualization
- **Clean Modern Interface**: Black background with professional color scheme matching industry tools like OpenBoardView and Altium
- **High-Quality Rendering**: Solid component fills, proper color coding, and clear pin visibility
- **Smart Zoom**: Pin labels become visible at high zoom levels for detailed inspection

### Interactive Pin Selection
- **Double-Click Selection**: Double-click any pin to select it with a yellow glowing effect
- **Net Highlighting**: When a pin is selected, all pins on the same electrical net are highlighted with the same yellow glow
- **Single Selection Mode**: Only one pin can be selected at a time; clicking empty space deselects
- **Visual Feedback**: Selected pins have a distinctive yellow glow effect that makes them easy to identify

### Information Display
- **Hover Tooltips**: Hover over any pin to see basic information (pin name, component)
- **Details Panel**: When a pin is selected, a sidebar shows detailed information including:
  - Pin name and component reference
  - Net name and connectivity information
  - List of all other pins connected to the same net
- **Real-time Updates**: Information updates immediately as you select different pins

### File Format Support
- **XZZPCB Format**: Native support for XZZPCB files
- **Multiple Test Files**: Includes sample files for testing and demonstration

## Quick Start

1. **Build the application**:
   ```bash
   # Windows
   build_clean.bat
   
   # Linux/macOS  
   ./build_clean.sh
   ```

2. **Run the viewer**:
   ```bash
   # The executable will be in:
   build/Release/pcb_viewer.exe  # Windows
   build/pcb_viewer              # Linux/macOS
   ```

3. **Load a PCB file**:
   - Use File → Open to load a .xzzpcb file
   - Sample files are available in the `test_files/` directory

## Usage

### Navigation
- **Pan**: Click and drag to move around the PCB
- **Zoom**: Use mouse wheel to zoom in/out
- **Fit View**: The view automatically fits the PCB on load

### Pin Selection and Net Analysis
- **Select Pin**: Double-click on any pin to select it
- **View Net**: Selected pin and all connected pins glow yellow
- **Pin Information**: Check the details panel for connectivity information
- **Deselect**: Click on empty space to clear selection

### Professional Features
- **Component Identification**: Different colors for different component types
- **Pin Visibility**: Pin labels appear at appropriate zoom levels
- **Net Tracing**: Visual connection highlighting for circuit analysis

## Technical Details

### Architecture
- **Modern C++17**: Clean, maintainable codebase
- **OpenGL Rendering**: Hardware-accelerated graphics
- **ImGui Interface**: Immediate mode GUI for responsive controls
- **Cross-Platform**: Runs on Windows, Linux, and macOS

### Dependencies
- GLFW (window management)
- GLEW (OpenGL extensions)
- ImGui (user interface)
- OpenGL (graphics rendering)

## Building from Source

See [BUILD.md](BUILD.md) for detailed build instructions and system requirements.

## Contributing

This project aims to provide a professional-grade PCB viewer with features comparable to commercial tools. Contributions for additional file formats, visualization improvements, or analysis features are welcome.

## License

[License information to be added]

## Acknowledgments

- **OpenBoardView**: For inspiring the PCB visualization features
- **Altium**: For the professional color scheme reference
- **GLFW, GLEW, ImGui**: For their excellent open-source libraries

