#!/usr/bin/env python3
"""
Create a more complex XZZPCB file for testing the PCB viewer
"""

def create_demo_xzzpcb():
    # Create binary data for a more complex XZZPCB file
    data = bytearray()
    
    # Add XZZPCB header (6 bytes)
    data.extend(b"XZZPCB")
    
    # Add version and header info (make it > 100 bytes for sample data)
    data.extend(b"\x00\x01")  # Version
    data.extend(b"\x00\x00")  # Flags
    data.extend(b"\x00\x00\x00\x00")  # Reserved
    
    # Add more data to make file > 100 bytes so sample data triggers
    # PCB outline data (more complex shape)
    for i in range(20):  # Add 20 outline segments
        data.extend(b"\x01")  # Outline command
        x1 = i * 500
        y1 = 100 if i % 2 == 0 else 200
        x2 = (i + 1) * 500
        y2 = 200 if i % 2 == 0 else 100
        
        data.extend(x1.to_bytes(4, 'little'))
        data.extend(y1.to_bytes(4, 'little'))
        data.extend(x2.to_bytes(4, 'little'))
        data.extend(y2.to_bytes(4, 'little'))
    
    # Add component data
    for i in range(10):  # Add 10 components
        data.extend(b"\x02")  # Component command
        x = 1000 + (i * 1200)
        y = 1500 + ((i % 3) * 800)
        
        data.extend(x.to_bytes(4, 'little'))
        data.extend(y.to_bytes(4, 'little'))
        data.extend((800).to_bytes(4, 'little'))  # Width
        data.extend((600).to_bytes(4, 'little'))  # Height
    
    # Add pin data
    for i in range(50):  # Add 50 pins
        data.extend(b"\x03")  # Pin command
        x = 500 + (i * 200)
        y = 500 + ((i % 5) * 150)
        
        data.extend(x.to_bytes(4, 'little'))
        data.extend(y.to_bytes(4, 'little'))
        data.extend((80).to_bytes(4, 'little'))   # Radius
    
    # Add padding to ensure > 100 bytes
    while len(data) < 120:
        data.extend(b"\x00")
    
    # End marker
    data.extend(b"\xFF\xFF\xFF\xFF")
    
    return data

if __name__ == "__main__":
    # Generate the demo file
    demo_data = create_demo_xzzpcb()
    
    # Write to file
    with open("test_files/complex_demo.xzzpcb", "wb") as f:
        f.write(demo_data)
    
    print(f"Created complex_demo.xzzpcb ({len(demo_data)} bytes)")
    print("File contains more complex PCB data for better visualization")
