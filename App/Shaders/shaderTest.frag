#version 460 core

layout(location = 0) in vec3 normal;
layout(location = 1) in vec3 view;
layout(location = 2) in vec3 light;
//layout(location = 1) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

uniform vec3 kd, ks, ka; // diffuse, specular, ambient
uniform float shine;
uniform vec3 La, Le;    // ambient and point source rad

//uniform vec3 u_ViewPosition;
//uniform vec3 u_LightPosition;
//uniform vec3 lightColor;
//uniform vec3 objectColor;
//uniform float specularStrength;

void main() {
    vec3 N = normalize(normal);
    vec3 V = normalize(view);
    vec3 L = normalize(light);
    vec3 H = normalize(L + V);
    float cost = max(dot(N,L), 0), cosd = max(dot(N,H), 0);
    vec3 color = ka * La + (kd * cost + ks * pow(cosd,shine)) * Le;
    outColor = vec4(color, 1);
    
//    // ambient
//    float ambientStrength = 0.1;
//    vec3 ambient = ambientStrength * lightColor;
//    
//    // diffuse 
//    vec3 norm = normalize(normal);
//    vec3 lightDir = normalize(u_LightPosition - fragPos);
//    float diff = max(dot(norm, lightDir), 0.0);
//    vec3 diffuse = diff * lightColor;
//
//    // specular
//    vec3 viewDir = normalize(u_ViewPosition - fragPos);
//    vec3 reflectDir = reflect(-lightDir, norm);
//    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//    vec3 specular = specularStrength * spec * lightColor;
//
//    vec3 result = (ambient + diffuse + specular) * objectColor;
//    outColor = vec4(result, 1.0);
}