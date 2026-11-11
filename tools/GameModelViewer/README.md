# GameModelViewer

A small C++20 / OpenGL 3.3 model-viewer foundation.

## Current status

- GLFW window and OpenGL 3.3 Core context
- GLAD OpenGL function loading
- Shader loading and compilation
- Vertex buffer / vertex array setup
- A coloured test triangle
- Escape closes the application

## Requirements

- Windows 11
- Visual Studio 2022 with the **Desktop development with C++** workload
- CMake 3.25 or newer
- Git
- vcpkg

## Setup

Open **Developer PowerShell for VS 2022**.

Clone the repository, then enter this directory:

```powershell
git clone https://github.com/artemoniy7/real-life-sim.git
cd real-life-sim/tools/GameModelViewer
```

Install vcpkg if it is not already installed:

```powershell
git clone https://github.com/microsoft/vcpkg.git C:\dev\vcpkg
& C:\dev\vcpkg\bootstrap-vcpkg.bat
$env:VCPKG_ROOT = "C:\dev\vcpkg"
```

If vcpkg is already installed elsewhere, set `VCPKG_ROOT` to that folder instead.

Configure and build:

```powershell
cmake --preset windows-vs2022
cmake --build --preset windows-debug
```

Run:

```powershell
.\build\Debug\GameModelViewer.exe
```

The first configure step downloads the dependencies declared in `vcpkg.json`.
You can also open this folder in Visual Studio 2022 using **File → Open → Folder**.

## Controls

- **Esc** — close the viewer

## Planned modules

1. Orbit camera and zoom
2. OBJ test-model loader
3. Texture loading, including DDS
4. Material and UV inspection
5. Dedicated loaders for documented game buffer layouts
