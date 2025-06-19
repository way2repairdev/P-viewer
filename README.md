# PCB OpenViewer - XZZPCB Format Support

A standalone OpenGL-based PCB viewer specifically designed for XZZPCB file format with DES decryption support.

## Features

- **XZZPCB Format Support**: Full support for XZZPCB files with DES decryption
- **OpenGL Rendering**: Hardware-accelerated rendering with zoom and pan
- **Interactive Controls**: Mouse-based navigation and keyboard shortcuts
- **Cross-Platform**: Built with CMake for Windows, Linux, and macOS

## Controls

- **Left Mouse Button + Drag**: Pan the view
- **Mouse Wheel**: Zoom in/out
- **R Key**: Reset view to fit PCB
- **Ctrl+O**: Open PCB file dialog
- **ESC Key**: Exit application

## Installation & Building

For detailed installation instructions, see [INSTALL.md](INSTALL.md).

### Quick Start

**Windows with vcpkg (Recommended):**
```cmd
build_windows_vcpkg.bat
```

**Windows without vcpkg:**
```cmd
build_windows.bat
```

**Linux/macOS:**
```bash
chmod +x build_unix.sh
./build_unix.sh
```

### Manual Build

```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

### Linux/macOS

```bash
mkdir build
cd build
cmake ..
make -j$(nproc)
```

## Usage

### Command Line

```bash
# Run with a specific XZZPCB file
./pcb_viewer path/to/your/file.xzz

# Run with sample data (no file specified)
./pcb_viewer
```

### Supported File Extensions

- `.xzz` - XZZPCB format files
- `.pcb` - Generic PCB format files

## Architecture

The viewer is organized into several modules:

- **Core**: Basic data types and utilities (`BRDTypes.h`, `Utils.h`)
- **Formats**: File format parsers (`XZZPCBFile.h`, `BRDFileBase.h`)
- **Renderer**: OpenGL rendering system (`PCBRenderer.h`, `Window.h`)
- **Main**: Application entry point and main loop

## File Format Support

Currently supports XZZPCB format with:

- DES decryption for encrypted files
- XOR decryption for obfuscated headers
- Part and pin parsing
- Board outline extraction
- Net information (partial)

## Dependencies

- **OpenGL**: Graphics rendering
- **GLFW**: Window management and input handling
- **GLEW**: OpenGL extension loading
- **DES**: Data Encryption Standard implementation

## License

This project builds upon the OpenBoardView codebase and maintains compatibility with its licensing terms.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test with sample XZZPCB files
5. Submit a pull request

## Troubleshooting

### Common Issues

1. **Missing OpenGL libraries**: Install graphics drivers and development packages
2. **GLFW/GLEW not found**: Install development packages or specify paths in CMake
3. **File format errors**: Ensure the file is a valid XZZPCB format

### Debug Information

The application provides console output for debugging:
- File loading status
- Rendering information
- Error messages with details

## Future Enhancements

- Additional PCB file format support
- Enhanced rendering features (layers, nets, annotations)
- 3D visualization capabilities
- Measurement tools
- Export functionality
