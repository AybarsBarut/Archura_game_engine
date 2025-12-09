#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;
// Instancing Attributes (4 locations used by mat4)
layout (location = 4) in mat4 aInstanceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec3 Color;

uniform mat4 uModel; // Fallback for non-instanced
uniform mat4 uView;
uniform mat4 uProjection;
uniform bool uUseInstancing;

void main() {
    mat4 model;
    if (uUseInstancing) {
        model = aInstanceMatrix;
    } else {
        model = uModel;
    }

    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    Color = aColor;
    
    gl_Position = uProjection * uView * vec4(FragPos, 1.0);
}
