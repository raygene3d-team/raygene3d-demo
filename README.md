<img src="https://github.com/raygene3d-team/raygene3d-demo/blob/readme/preview.jpg" width="120" height="120">

# RayGene Demo

RayGene is a developer-oriented cross GPU API framework written in portable C++17. Our goal is to make development of render systems more easily and faster.

The framework currently supports Vulkan and DirectX11 APIs and runs on Windows, Linux and MacOS platforms. DirectX12 and Metal support will be available later.

This project allows you to try the framework on a simple scene with different scenarios: indirect drawing, compute pipeline and depth-only rendering. Please follow the instructions to build and run the project.

## Requirements

- [Vulkan SDK](https://vulkan.lunarg.com/)
- [CMake 3.4](https://cmake.org/download/)

## Compiling and starting RayGene Demo

To build and run the project, clone this repository
```
    git clone https://github.com/raygene3d-team/raygene3d-demo.git --recursive
```
Make the project root directory as current
```
    cd raygene3d-demo
```

Note, that there are some pre-built dependencies we distribute. It imposes some software and hardware restrictions. Don't worry, we are working on it. Currently you need x86 architecture to build and run the demo. Software restrictions will be described below.

### Windows

1. Go to 3rdparty directory and unzip 3rdparty.zip to the directory
2. Run CMake GUI
3. Specify the source code field as root directory of the cloned repo, specify the output directory (pick root directory as cmake output) and press the Configure button.
4. Then select your Visual Studio version and configure the project.
5. Generate VS Project file and open it. As we said before we have some pre-build dependencies and you must use v142 toolset.
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
| **F2** | Enable/disable shadows |

## Third party headers and libs

- [glfw](https://github.com/glfw/glfw)
- [glm](https://github.com/g-truc/glm)
- [imgui](https://github.com/ocornut/imgui)
- [stb](https://github.com/nothings/stb)
- [tinyobj](https://github.com/tinyobjloader/tinyobjloader)
- [nlohmann](https://github.com/nlohmann/json)

## License

RayGene is licensed under the [MIT license](https://github.com/raygene3d-team/raygene3d-demo/blob/main/LICENSE)