cmake_minimum_required(VERSION 3.15)
project(PCBOpenViewer VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set up local vcpkg paths
set(VCPKG_INSTALLED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg_installed/x64-windows")
set(CMAKE_PREFIX_PATH "${VCPKG_INSTALLED_DIR};${VCPKG_INSTALLED_DIR}/share")

# Add include directories
include_directories("${VCPKG_INSTALLED_DIR}/include")
link_directories("${VCPKG_INSTALLED_DIR}/lib")
link_directories("${VCPKG_INSTALLED_DIR}/debug/lib")

# Find packages
find_package(OpenGL REQUIRED)

# Include directories
include_directories(src)
include_directories(src/core)
include_directories(src/formats)
include_directories(src/renderer)

# Source files
set(CORE_SOURCES
    src/core/BRDTypes.cpp
    src/core/Utils.cpp
)

set(FORMAT_SOURCES
    src/formats/BRDFileBase.cpp
    src/formats/XZZPCBFile.cpp
    src/formats/des.cpp
)

set(RENDERER_SOURCES
    src/renderer/PCBRenderer.cpp
    src/renderer/Window.cpp
)

set(MAIN_SOURCES
    src/main.cpp
)

# Create executable
add_executable(pcb_viewer
    ${CORE_SOURCES}
    ${FORMAT_SOURCES}
    ${RENDERER_SOURCES}
    ${MAIN_SOURCES}
)

# Link libraries directly
target_link_libraries(pcb_viewer
    ${OPENGL_LIBRARIES}
    glfw3dll
    glew32
    imgui
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(pcb_viewer
        comdlg32
    )
endif()

# Compiler-specific options
if(MSVC)
    target_compile_definitions(pcb_viewer PRIVATE
        _CRT_SECURE_NO_WARNINGS
        NOMINMAX
    )
endif()

# Copy test files to build directory
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test_files DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
