#version 460 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 v_FragPos;
    vec3 v_Normal;
} gs_in[];

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 fragColor;

void GenerateNormal() {
    vec4 center = vec4(0.0);
    for (int i = 0; i < 3; i++) {
        center += gl_in[i].gl_Position;
    }
    gl_Position = u_Projection * center;
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    EmitVertex();

    gl_Position = u_Projection * (center + normalize(vec4(gs_in[0].v_Normal, 1.0)));
    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    EmitVertex();
    EndPrimitive();
}

void GenerateLightVec() {
    vec4 center = vec4(0.0);
    for (int i = 0; i < 3; i++) {
        center += gl_in[i].gl_Position;
    }

    gl_Position = u_Projection * center;
    fragColor = vec4(0.0, 1.0, 0.0, 1.0);
    EmitVertex();

    vec4 lightPosNDC = vec4(u_View * u_Model * vec4(lightPos, 1.0));
    gl_Position = u_Projection * (center + normalize(lightPosNDC - center));
    fragColor = vec4(0.0, 1.0, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}

void main() {
    GenerateNormal();
    GenerateLightVec();
}