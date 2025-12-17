#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aColor;
// Instancing Attributes (4 locations used by mat4)
layout (location = 4) in mat4 aInstanceMatrix;
layout (location = 8) in ivec4 aBoneIDs;
layout (location = 9) in vec4 aWeights;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 uFinalBonesMatrices[MAX_BONES];
uniform bool uHasAnimation;

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

    vec4 totalPosition = vec4(0.0f);
    vec3 localNormal = vec3(0.0f); // Accumulate skinned normal

    if (uHasAnimation) {
        for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++) {
            if(aBoneIDs[i] == -1) continue;
            if(aBoneIDs[i] >= MAX_BONES) {
                totalPosition = vec4(aPos,1.0f);
                break;
            }
            vec4 localPos = uFinalBonesMatrices[aBoneIDs[i]] * vec4(aPos,1.0f);
            totalPosition += localPos * aWeights[i];
            
            // Skin Normal
            vec3 worldNormal = mat3(uFinalBonesMatrices[aBoneIDs[i]]) * aNormal;
            localNormal += worldNormal * aWeights[i];
        }
        
        // Fallback for unweighted vertices (static parts of a mesh?)
        if (length(totalPosition) < 0.0001) totalPosition = vec4(aPos, 1.0f);
    } else {
        totalPosition = vec4(aPos, 1.0f);
        localNormal = aNormal;
    }

    FragPos = vec3(model * totalPosition);
    // Normal matrix must serve orientation. If animated, localNormal is already deformed.
    // But we still need to apply Model rotation (if character rotates in world).
    // Note: uFinalBonesMatrices acts as "bind pose -> current pose". It usually includes root transform?
    // Often it does NOT include World Transform (Model). So we apply Model * Skin.
    Normal = mat3(transpose(inverse(model))) * (uHasAnimation ? localNormal : aNormal);
    TexCoords = aTexCoords;
    Color = aColor;
    
    Color = aColor;
    
    gl_Position = uProjection * uView * vec4(FragPos, 1.0);
}
