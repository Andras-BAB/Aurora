#version 460 core

layout(location = 0) in vec3 v_FragPos;
layout(location = 1) in vec3 v_Normal;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

layout(location = 0) out vec4 outColor;

void main() {
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 norm = normalize(v_Normal);
    vec3 lightDir = normalize(lightPos - v_FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - v_FragPos);
    vec3 specular = vec3(0.0);
    if(dot(v_Normal, lightDir) > 0) {
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        specular = specularStrength * spec * lightColor;
    }
    
    vec3 result = (ambient + diffuse + specular) * objectColor;
    outColor = vec4(result, 1.0);
}
