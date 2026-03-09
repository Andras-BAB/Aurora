#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;

//layout(location = 0) out vec3 v_Normal;
out VS_OUT {
    vec3 v_Normal;
} vs_out;

void main() {
    mat3 normalMatrix = mat3(transpose(inverse(u_View * u_Model)));
    vs_out.v_Normal = vec3(vec4(normalMatrix * a_Normal, 1.0));
    gl_Position = u_View * u_Model * vec4(a_Position, 1.0);
}
