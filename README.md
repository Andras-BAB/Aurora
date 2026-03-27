# Aurora Engine (WIP)

A modern, DirectX 12-based 3D game engine focused on flexibility and low-level API performance. The project is currently under active development (Work in Progress). Started as a hobby project.

## Technical Overview & Features

The core of the engine is built on modern rendering concepts:

* API: DirectX 12 (3D)
* Immediate Mode Render Graph: For dynamic and flexible render pass management.
* Architecture:
    * Bindless Textures: Efficient resource management and modern texture access in shaders.
    * Global Vertex and Index Buffers: To minimize state changes and draw call overhead.
    * Uber Root Signature: Unified pipeline state and resource binding.
* Rendering Pipeline: Currently using Forward Rendering, but the architecture (specifically the Render Graph) is designed to allow a seamless transition to Deferred Rendering in the future.

## Roadmap / Planned Features

As a WIP project, the immediate focus is on implementing the following:

- [ ] Lot of rendering changes e.g. frustum culling, multiple global buffers (position only for shadow pass), draw call reordering for materials, better GPU heap usage with descriptor tables (and for bindless textures) instead of root constants for objects
- [ ] Better asset pipeline, and model loading with textures
- [ ] Transitioning to a Deferred Rendering pipeline utilizing the Render Graph.
- [ ] Shadow pass implementation (currently no shadows).
- [ ] Scripting with Lua
- [ ] Simple UI


## Build and Run

It uses `premake5`. Run the `Scripts/Setup-Windows.bat`. That will generate Visual Studio 2026 files.

All dependencies are included in the repository.