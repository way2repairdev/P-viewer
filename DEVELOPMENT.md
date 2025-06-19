# PCB OpenViewer - Development Guide

This document provides guidance for developers working on the PCB OpenViewer project.

## Project Structure

```
openviewer/
├── CMakeLists.txt          # Main CMake configuration
├── vcpkg.json             # vcpkg dependency manifest
├── build_windows.bat      # Windows build script
├── build_windows_vcpkg.bat # Windows build script with vcpkg
├── build_unix.sh          # Linux/macOS build script
├── README.md              # User documentation
├── DEVELOPMENT.md         # This file
├── test_files/            # Test XZZPCB files
└── src/
    ├── main.cpp           # Application entry point
    ├── core/              # Core data structures and utilities
    │   ├── BRDTypes.h/cpp    # PCB data structures
    │   └── Utils.h/cpp       # Utility functions and logging
    ├── formats/           # File format parsers
    │   ├── BRDFileBase.h/cpp # Base class for file formats
    │   ├── XZZPCBFile.h/cpp  # XZZPCB format parser
    │   └── des.h/cpp         # DES decryption
    └── renderer/          # OpenGL rendering
        ├── Window.h/cpp      # GLFW window management
        └── PCBRenderer.h/cpp # PCB rendering logic
```

## Key Components

### Core Data Structures (`src/core/`)

- **BRDTypes**: Defines fundamental PCB data structures
  - `BRDPoint`: 2D coordinates in mils/thou
  - `BRDPart`: PCB component information
  - `BRDPin`: Component pin information
  - `BRDNail`: Test point information

- **Utils**: Provides logging and utility functions
  - `LOG_INFO`, `LOG_ERROR`, `LOG_WARNING` macros
  - File I/O helper functions

### File Format Support (`src/formats/`)

- **BRDFileBase**: Abstract base class for all file format parsers
  - Pure virtual `Load()` and `VerifyFormat()` methods
  - Common PCB data storage

- **XZZPCBFile**: XZZPCB format parser with DES decryption
  - Handles encrypted XZZPCB files
  - Supports various block types (arcs, lines, parts, test pads)
  - Chinese character encoding support (GB2312)

- **DES**: DES decryption implementation for XZZPCB files

### Rendering System (`src/renderer/`)

- **Window**: GLFW-based window management
  - OpenGL context creation
  - Input event handling
  - Callback system for user interaction

- **PCBRenderer**: OpenGL-based PCB rendering
  - 2D rendering with zoom and pan
  - Renders PCB outlines, parts, pins, and test points
  - Color-coded rendering for different component types

## Adding New File Formats

To add support for a new PCB file format:

1. Create new parser class inheriting from `BRDFileBase`
2. Implement `Load()` and `VerifyFormat()` methods
3. Parse file data into common `BRDTypes` structures
4. Add format detection to main application
5. Update CMakeLists.txt to include new source files

Example:
```cpp
class NewFormatFile : public BRDFileBase {
public:
    bool Load(const std::vector<char>& buffer, const std::string& filepath = "") override;
    bool VerifyFormat(const std::vector<char>& buffer) override;
    static std::unique_ptr<NewFormatFile> LoadFromFile(const std::string& filepath);
};
```

## Rendering Enhancements

The rendering system is designed to be extensible:

### Adding New Visual Elements

1. Extend `BRDTypes` with new data structures if needed
2. Add rendering methods to `PCBRenderer`
3. Update color schemes and visual styling

### Performance Optimizations

- Use OpenGL vertex buffer objects (VBOs) for large datasets
- Implement level-of-detail (LOD) rendering for complex boards
- Add frustum culling for off-screen elements

## Building and Testing

### Dependencies

- **OpenGL**: Hardware-accelerated graphics
- **GLFW3**: Cross-platform windowing and input
- **GLEW**: OpenGL extension loading
- **CMake 3.15+**: Build system
- **C++17**: Modern C++ features

### Build Configurations

- **Debug**: Full debugging symbols, assertions enabled
- **Release**: Optimized for performance
- **RelWithDebInfo**: Optimized with debug info

### Testing Strategy

1. **Unit Tests**: Test individual components (file parsers, utilities)
2. **Integration Tests**: Test complete file loading and rendering
3. **Visual Tests**: Verify rendering output against reference images
4. **Performance Tests**: Measure loading and rendering performance

## Code Style Guidelines

- Use C++17 features where appropriate
- Follow RAII principles for resource management
- Use smart pointers (`std::unique_ptr`, `std::shared_ptr`)
- Prefer `const` correctness
- Use meaningful variable and function names
- Include comprehensive error handling

### Logging

Use the provided logging macros:
```cpp
LOG_INFO("Successfully loaded PCB file: " << filename);
LOG_WARNING("Unrecognized block type: " << block_type);
LOG_ERROR("Failed to decrypt file: " << error_message);
```

## Future Enhancements

### Planned Features

1. **Multi-layer Support**: Render different PCB layers
2. **3D Visualization**: Extrude components for 3D view
3. **Measurement Tools**: Distance and area measurement
4. **Export Functionality**: Save rendered images and data
5. **Plugin System**: Load additional file format plugins
6. **Advanced Navigation**: Mini-map, bookmarks, search

### File Format Priorities

1. **Gerber**: Industry standard for PCB manufacturing
2. **KiCad**: Open-source EDA tool format
3. **Altium**: Commercial EDA tool format
4. **Eagle**: Autodesk EAGLE format

## Contributing

1. Fork the repository
2. Create a feature branch
3. Implement changes with tests
4. Ensure code follows style guidelines
5. Submit pull request with detailed description

## Performance Considerations

- **Memory Usage**: Use streaming for large files
- **Rendering**: Implement view frustum culling
- **Threading**: Consider background file loading
- **Caching**: Cache parsed file data for quick reloading

## Debugging Tips

- Use OpenGL debug callbacks for rendering issues
- Enable verbose logging for file parsing problems
- Use graphics debuggers (RenderDoc, Nsight Graphics)
- Profile with tools like Intel VTune or Visual Studio Diagnostics
