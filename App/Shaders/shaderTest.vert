#version 460 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;

layout(location = 0) out vec3 normal;
//layout(location = 1) out vec3 fragPos;
layout(location = 1) out vec3 view;
layout(location = 2) out vec3 light;

uniform mat4 u_Model;
uniform mat4 u_ViewProjection;
uniform vec4 u_LightPos;
uniform vec3 u_EyePos;

void main() {
//    fragPos = vec3(u_Model * vec4(inPosition, 1.0));
//    normal = mat3(transpose(inverse(u_Model))) * inNormal;

    gl_Position = u_ViewProjection * u_Model * vec4(inPos, 1.0);

    vec4 wPos = vec4(inPos, 1) * u_Model;
    light  = u_LightPos.xyz / u_LightPos.w - wPos.xyz * wPos.w;
    view   = u_EyePos - wPos.xyz / wPos.w;
    normal = (inverse(u_Model) * vec4(inNormal, 0)).xyz;
}