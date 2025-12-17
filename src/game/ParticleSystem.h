#pragma once

#include "../ecs/System.h"
#include <glm/glm.hpp>

namespace Archura {

class ParticleSystem : public System {
public:
    ParticleSystem() = default;
    ~ParticleSystem() = default;

    void Update(float deltaTime) override;
    
    // Helper to spawn a burst of particles
    void EmitBurst(
        Scene* scene, 
        const glm::vec3& position, 
        const glm::vec3& normal, // For direction bias
        int count,
        glm::vec4 color, 
        float speed, 
        float size,
        float lifetime,
        bool useGravity = true
    );
};

} // namespace Archura
