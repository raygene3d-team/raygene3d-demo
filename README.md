<img src="https://github.com/raygene3d-team/raygene3d-demo/blob/readme/preview.jpg" width="120" height="120">

# RayGene Demo

Demo is powered by RayGene and utilizes its most common rendering features. It presents Crytek Sponza scene including one point light with omni-shadowing and ImGUI wrapper.

Demo shows how to use:
- direct/indirect drawing
- compute/raster pipelines
- static/dynamic GPU resource management
- depth-only and forward passes

## About RayGene

RayGene is a developer-oriented graphics framework written in portable C++17. It unifies dealing with a variety of graphics APIs and speeds up engineering of render systems.

The framework currently supports Vulkan and DirectX11 APIs and runs on Windows, Linux and MacOS platforms. DirectX12 and Metal support will be available later.

## Requirements

- [Vulkan SDK](https://vulkan.lunarg.com/)
- [CMake 3.4](https://cmake.org/download/)

## Compiling and Running

To build and run the project, clone this repository
```
    git clone https://github.com/raygene3d-team/raygene3d-demo.git --recursive
```
Make the project root directory as current
```
    cd raygene3d-demo
```

Please note, that there are pre-built binaries for 64-bit x86 architectures only. These dependencies are distributed with zipped archive and can be found in the 3rdparty directory.

### Windows

1. Go to 3rdparty directory and unzip 3rdparty.zip to the directory
2. Run CMake GUI
3. Specify the source code field as root directory of the cloned repo, specify the output directory (pick root directory as cmake output) and press the Configure button.
4. Then select your Visual Studio version and configure the project.
5. Generate VS Project file and open it. As we said before we have some pre-built dependencies and you must use v142 toolset.
6. Compile and run the project

### Linux and MacOS

Unzip 3rdparty.zip
```
    cd 3rdparty
    unzip 3rdparty.zip
    cd ..
```
Confugure the project with the command
```
    cmake -DCMAKE_BUILD_TYPE=Release .
```
Build the project
```
    make -j4
```
And run from the root directory
```
    ./bin/raygene3d-demo
```

## Controls

| Key | |
| :--: | :-- |
| **WASD+RF** | Camera movement |
| **F1** | Enable/Disable ImGUI |
| **F2** | Enable/Disable shadows |

## Third party headers and libs

- [glfw](https://github.com/glfw/glfw)
- [glm](https://github.com/g-truc/glm)
- [imgui](https://github.com/ocornut/imgui)
- [stb](https://github.com/nothings/stb)
- [tinyobj](https://github.com/tinyobjloader/tinyobjloader)
- [nlohmann](https://github.com/nlohmann/json)

## License

RayGene is licensed under the [MIT license](https://github.com/raygene3d-team/raygene3d-demo/blob/main/LICENSE)