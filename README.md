# Aurora Engine

A modern, DirectX 12-based 3D game and rendering engine focused on flexibility and low-level API performance. The project is currently under active development and in early phase. Started as a hobby project.

## Build and run

It uses `premake5`. Run the `Scripts/Setup-Windows.bat`. That will generate Visual Studio 2026 files.

All dependencies are included in the repository.

## Technical overview & features

The core of the engine is built on modern rendering concepts:

* Immediate mode render graph: for dynamic and flexible render pass management
* Bindless GPU resource management
* Rendering pipeline: currently using forward rendering, but the architecture (specifically the render graph) is designed to allow a seamless transition to deferred rendering in the future

## Planned features
*This section can change.*

As a WIP project, the main focus is on implementing the following:

* [ ] Scene saving and loading
* [ ] Better asset pipeline, and model loading with textures
* [ ] Transitioning to a deferred rendering pipeline utilizing the render graph.
* [ ] Shadow pass implementation (currently no shadows).
* [ ] Lot of rendering changes e.g. frustum culling, multiple global buffers (position only for shadow pass), draw call reordering for materials, better GPU heap usage with descriptor tables (and for bindless textures) instead of root constants for objects
* [ ] Scripting with Lua
* [ ] Simple UI
* [ ] GPU based particle system
