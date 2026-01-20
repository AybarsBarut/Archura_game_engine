#pragma once

#include "../ecs/Component.h"
#include <glm/glm.hpp>
#include <glm/glm.hpp>
#include <vector>
#include "SurfaceProperty.h"

namespace Archura {

class Scene;
class Entity;
class ProjectileSystem;

/**
 * @brief Projectile component - Mermi/roket fizik ve data
 */
struct Projectile : public Component {
    enum class ProjectileType {
        Bullet,
        Grenade
    };

    ProjectileType type = ProjectileType::Bullet;
    glm::vec3 velocity;       // Hiz vektoru
    float speed = 50.0f;      // m/s
    float damage = 25.0f;
    float lifetime = 5.0f;    // Saniye (sonra yok ol)
    float gravity = 0.0f;     // Gravity effect (0 = no gravity, -9.81 = realistic)
    bool hasHit = false;
    
    // Grenade specific
    float fuseTimer = 5.0f;
    float explosionRadius = 5.0f;
    
    Entity* owner = nullptr;  // Projectile'i atan entity (self-hit onlemek icin)
};



} // namespace Archura
