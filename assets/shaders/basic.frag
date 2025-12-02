#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in vec3 Color;

out vec4 FragColor;

// Texture
uniform sampler2D uTexture;
uniform bool uUseTexture;

// Lighting uniforms
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform vec3 uViewPos;

// Material properties
uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uShininess;

void main() {
    // Base color - texture veya vertex color
    vec3 baseColor = uUseTexture ? texture(uTexture, TexCoords).rgb : Color;
    
    // Ambient
    vec3 ambient = uAmbient * uLightColor;
    
    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = uDiffuse * diff * uLightColor;
    
    // Specular (Blinn-Phong)
    vec3 viewDir = normalize(uViewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), uShininess);
    vec3 specular = uSpecular * spec * uLightColor;
    
    // Combine
    vec3 result = (ambient + diffuse + specular) * baseColor;
    FragColor = vec4(result, 1.0);
}
