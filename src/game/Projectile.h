#pragma once

#include "../ecs/Component.h"
#include <glm/glm.hpp>
#include <vector>

namespace Archura {

class Scene;
class Entity;

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

/**
 * @brief ProjectileSystem - Mermi hareketi ve collision
 */
class ProjectileSystem {
public:
    ProjectileSystem() = default;
    ~ProjectileSystem() = default;

    void Update(Scene* scene, float deltaTime);
    
    // Yeni projectile spawn et
    Entity* SpawnProjectile(
        Scene* scene,
        const glm::vec3& position,
        const glm::vec3& direction,
        float speed,
        float damage,
        Entity* owner = nullptr,
        Projectile::ProjectileType type = Projectile::ProjectileType::Bullet
    );

private:
    void UpdateProjectile(Entity* entity, Projectile* proj, float deltaTime);
    bool CheckCollision(Entity* projectile, Scene* scene);

private:
    std::vector<Entity*> m_ProjectilesToDestroy;
};

} // namespace Archura
