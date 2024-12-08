#version 460

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

uniform mat4 u_Model;
//uniform mat4 u_View;
//uniform mat4 u_Projection;
uniform mat4 u_ViewProjection;

void main() {
//    mat4 modelMat = mat4(1.0);
    
    // VIEW MATRIX
//    mat4 viewMat = mat4(1.0); // Egységmátrix
//    vec3 cameraPos = vec3(1.0, 2.0, 3.0); // Kamera pozíció (hátrébb a z-tengelyen)
//    vec3 target = vec3(0.0, 0.0, 0.0); // Hova néz a kamera
//    vec3 up = vec3(0.0, 1.0, 0.0); // Felfelé irány (y-tengely)
//
//    vec3 zAxis = normalize(cameraPos - target);
//    vec3 xAxis = normalize(cross(up, zAxis));
//    vec3 yAxis = cross(zAxis, xAxis);
//
//    viewMat = mat4(
//        vec4(xAxis, 0.0),
//        vec4(yAxis, 0.0),
//        vec4(zAxis, 0.0),
//        vec4(-dot(xAxis, cameraPos), -dot(yAxis, cameraPos), -dot(zAxis, cameraPos), 1.0)
//        );
//    
//    // PROJECTION MATRIX
//    mat4 projMat = mat4(1.0);
//    float fov = 45.0; // Látószög (fokban)
//    float aspect = 1600.0 / 900.0; // Aspect ratio
//    float near = 0.1; // Közelítési távolság
//    float far = 100.0; // Távoli távolság
//
//    float tanHalfFov = tan(radians(fov) / 2.0);
//    projMat = mat4(
//        1.0 / (aspect * tanHalfFov), 0.0, 0.0, 0.0,
//        0.0, 1.0 / tanHalfFov, 0.0, 0.0,
//        0.0, 0.0, -(far + near) / (far - near), -1.0,
//        0.0, 0.0, -(2.0 * far * near) / (far - near), 0.0
//    );
    
    //gl_Position = u_Projection * u_View * u_Model * vec4(inPosition, 1.0);
    gl_Position = u_ViewProjection * u_Model * vec4(inPosition, 1.0);
    fragColor = inColor;
}