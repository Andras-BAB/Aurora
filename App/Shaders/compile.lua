local loc = os.getenv("VULKAN_SDK")
local fullVert = loc .. "\\Bin\\glslc.exe shader.vert -o vert.spv"
local fullFrag = loc .. "\\Bin\\glslc.exe shader.frag -o frag.spv"
os.execute(fullVert)
os.execute(fullFrag)

print("Shaders compiled")