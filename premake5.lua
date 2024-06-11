
-- premake5.lua
workspace "Aurora"
   architecture "x64"
   configurations { "Debug", "Release" }
   startproject "App"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }
   filter {}

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

newoption {
   trigger = "gfxapi",
   value = "API",
   description = "Choose a particular 3D API for rendering",
   allowed = {
      { "opengl",    "OpenGL" },
      { "vulkan",  "Vulkan" },
      { "direct3d",  "Direct3D (Windows only)" }
   },
   default = "opengl"
}

include "Dependencies.lua"

include "App/premake5.lua"