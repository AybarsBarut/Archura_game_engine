#pragma once

#include "../ecs/Component.h"
#include <glm/glm.hpp>

namespace Archura {

/**
 * @brief Particle component - Visual effect particle
 */
struct Particle : public Component {
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 acceleration = glm::vec3(0.0f); // e.g. Gravity
    glm::vec4 color = glm::vec4(1.0f); // RGBA
    float size = 0.1f;
    float lifetime = 1.0f;
    float startLifetime = 1.0f; // For interpolation
    
    // Simple logic: true = billboard, false = mesh
    bool billboard = true;
};

} // namespace Archura
