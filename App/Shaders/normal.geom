#version 460 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 v_Normal;
} gs_in[];

const float MAGNITUDE = 0.2;

uniform mat4 u_Projection;

void GenerateLine(int index) {
    gl_Position = u_Projection * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = u_Projection * (gl_in[index].gl_Position + vec4(gs_in[index].v_Normal, 1.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main() {
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}