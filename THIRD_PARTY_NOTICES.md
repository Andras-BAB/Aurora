# Third-Party Licenses and Acknowledgements

This project utilizes open-source components and architectural concepts from others.

## Hazel Engine

Portions of this engine's core foundation (specifically the Windowing, Event handling, Layer Stack, and basic Logging systems) are derived from the Hazel Engine created by The Cherno.

* Repository: [https://github.com/TheCherno/Hazel](https://github.com/TheCherno/Hazel)
* License: Apache License 2.0

**Notice of Modifications:** The original Hazel codebase has been significantly modified to fit the specific needs of this engine.
Modifications include, but are not limited to:
- Changing namespaces and naming conventions to match this project's standard.
- Complete removal of the 2D OpenGL rendering backend.
- Implementation of a custom 3D DirectX 12 backend to replace the original rendering architecture.

*Note: Since it's based on public Hazel 2D OpenGL version, the actual rendering backend and its associated logic have been entirely rewritten and re-architected from the ground up to serve the specific requirements of 3D rendering and DirectX 12.*

***Apache License 2.0***
http://www.apache.org/licenses/LICENSE-2.0

---

## Educational References

### Introduction to 3D game programming with DirectX 12

The DirectX 12 rendering architecture and implementation details were heavily inspired by and learned from the concepts presented in Frank Luna's book.

* Reference Repository: [https://github.com/d3dcoder/d3d12book](https://github.com/d3dcoder/d3d12book)

**Note:** The code has been almost completely integrated, rewritten, and adapted to function within this engine's unique component-based and render-graph-driven architecture.

