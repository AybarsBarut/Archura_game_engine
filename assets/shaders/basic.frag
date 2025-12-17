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
// Light struct
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
    float range;
    int type; // 0 = Directional, 1 = Point
};

#define MAX_LIGHTS 4
uniform Light uLights[MAX_LIGHTS];
uniform int uLightCount;

uniform vec3 uViewPos;

// Material properties
uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uShininess;

uniform sampler2D uShadowMap;
uniform mat4 uLightSpaceMatrix;

float ShadowCalculation(vec4 fragPosLightSpace) {
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(uShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    // float bias = 0.005; // Bias to prevent acne
    // Simple bias based on normal
    // vec3 normal = normalize(Normal);
    // vec3 lightDir = normalize(vec3(-0.2, -1.0, -0.3)); // Same as hardcoded dir
    // float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    float bias = 0.005;

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main() {
    // Base color
    vec3 baseColor = uUseTexture ? texture(uTexture, TexCoords).rgb : Color;
    
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(uViewPos - FragPos);
    
    vec3 totalLight = vec3(0.0);
    
    // Ambient (global, once)
    vec3 ambient = uAmbient * 0.1; // Reduced ambient base
    totalLight += ambient;

    for(int i = 0; i < uLightCount; i++) {
        vec3 lightDir;
        float attenuation = 1.0;
        float shadow = 0.0;

        if (uLights[i].type == 0) { // Directional
             lightDir = normalize(vec3(-0.2, -1.0, -0.3)); 
             // Calculate Shadow only for Directional
             vec4 fragPosLightSpace = uLightSpaceMatrix * vec4(FragPos, 1.0);
             shadow = ShadowCalculation(fragPosLightSpace);
        } else if (uLights[i].type == 1) { // Point
            lightDir = normalize(uLights[i].position - FragPos);
            float distance = length(uLights[i].position - FragPos);
            if (distance < uLights[i].range) {
                float a = distance / uLights[i].range;
                attenuation = 1.0 - a;
                attenuation = max(attenuation, 0.0);
                attenuation *= attenuation;
            } else {
                attenuation = 0.0;
            }
        } else if (uLights[i].type == 2) { // Circle (Ambient / Everywhere)
            totalLight += uLights[i].color * uLights[i].intensity;
            continue; 
        }
        
        // Diffuse
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = uDiffuse * diff * uLights[i].color * uLights[i].intensity;
        
        // Specular
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), uShininess);
        vec3 specular = uSpecular * spec * uLights[i].color * uLights[i].intensity;
        
        totalLight += (diffuse + specular) * attenuation * (1.0 - shadow);
    }
    
    vec3 result = totalLight * baseColor;
    FragColor = vec4(result, 1.0);
}
