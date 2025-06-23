This directory contains test files for the PCB OpenViewer.

## Test Files

Currently, this directory is empty. To test the viewer:

1. **With XZZPCB files**: Place your `.xzzpcb` files in this directory
2. **Without files**: The viewer will generate sample demo data to show functionality

## Usage

Run the viewer with a test file:
```
pcb_viewer.exe test_files/your_file.xzzpcb
```

Or run without arguments to see demo data:
```
pcb_viewer.exe
```

## Creating Test Files

If you have actual XZZPCB files from board manufacturers, place them here for testing.
The viewer supports DES-encrypted XZZPCB files commonly used in the electronics industry.
