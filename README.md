# Build
### CMake
Asura Engine using CMake build system.
```cmd 
cmake -B build
cmake --build build
```
### CI 
![example workflow](https://github.com/ForserX/Asura2D/actions/workflows/cmake.yml/badge.svg)

|__OS__|Windows|MacOS|Linux|FreeBSD|
|---|---|---|---|---|
|__Support__| *__Win10__* | *__10.15 "Catalina"__* | *__Ubuntu 18.04__* | WIP |
|__Compiler__| *MVSC, Clang* | *Clang* | *GCC*, *Clang*| |
|__C++__| Latest, 20 | 20 | 20 | 20 |

# Asura Engine 
Asura Engine is a 2D game engine that focuses on physical objects, rendering with geometric primitives or textures.

![image](https://user-images.githubusercontent.com/13867290/209982198-a1d3a83a-ba33-43e8-b5c1-d1e61fbbce92.gif)


# 3rd Party
### Render/UI
* OpenGL + GLTF + [SOIL2](https://github.com/SpartanJ/SOIL2.git)
* [ImGui](https://github.com/ocornut/imgui)
### Kernel
* MIO
* [Function2](https://github.com/Naios/function2) (extended)
* [MiMalloc](https://github.com/microsoft/mimalloc)
### Audio
* [OpenAL Soft](https://github.com/kcat/openal-soft) | XAudio2 (Windows only)
* [Opus](https://github.com/xiph/opus) + [ogg](https://github.com/xiph/ogg) 
### Game Systems
* [entt](https://github.com/skypjack/entt)
* [Box2D](https://box2d.org) (extended)
