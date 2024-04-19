# Long March

## Description

This library is a fresh start for my graduate career.
I'm planning to implement any rendering, simulation algorithms, and other stuff that I'm interested in.

| Work To Do        | Current Status   | Section   |
|-------------------|------------------|-----------|
| Geometry          | Work in Progress | Grassland |
| Data Structures   | Work in Progress | Grassland |
| Vulkan Backend    | Work in Progress | Grassland |
| Visualizer        | Pending          | SnowMount |
| Variational Fluid | Pending          | SnowMount |
| Finite Element    | Pending          | SnowMount |
| Articulated Body  | Pending          | SnowMount |

## Build

This project is built with CMake. It also requires Vulkan SDK and vcpkg.

To build the project, you need to do CMake configuration with setting VCPKG_PATH to the path of vcpkg.
```bash
cmake -B build -S . -DVCPKG_PATH=/path/to/vcpkg
```

Then, you can build the project with CMake.
```bash
cmake --build build
```
