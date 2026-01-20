#pragma once

#include "../ecs/System.h"
#include "Projectile.h"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace Archura {

class Scene;
class Entity;

class ProjectileSystem : public System {
public:
    ProjectileSystem();
    ~ProjectileSystem() = default;

    void Init(Scene* scene);
    void Update(float deltaTime) override;

    void UpdateProjectile(Entity* entity, Projectile* proj, float deltaTime);
    bool CheckCollision(Entity* projectile, Scene* scene);
    void SpawnDecal(Scene* scene, const glm::vec3& position, const glm::vec3& normal, SurfaceType surfaceType);
    
    Entity* SpawnProjectile(Scene* scene, const glm::vec3& position, const glm::vec3& direction, 
                            float speed, float damage, Entity* owner, Projectile::ProjectileType type);

private:
    Scene* m_Scene;
    std::vector<Entity*> m_ProjectilesToDestroy;
};

} // namespace Archura
