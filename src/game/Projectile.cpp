#include "Projectile.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include "../rendering/Mesh.h"
#include "../core/ResourceManager.h"
#include <iostream>
#include <algorithm>

namespace Archura {

void ProjectileSystem::Update(Scene* scene, float deltaTime) {
    if (!scene) return;

    m_ProjectilesToDestroy.clear();
    m_ProjectilesToDestroy.reserve(10); // Pre-allocate for common case

    // Tüm projectile'ları güncelle
    for (auto& entityPtr : scene->GetEntities()) {
        auto* projectile = entityPtr->GetComponent<Projectile>();
        if (projectile) {
            UpdateProjectile(entityPtr.get(), projectile, deltaTime);
        }
    }

    // Vurmuş veya expired projectile'ları yok et
    for (auto* entity : m_ProjectilesToDestroy) {
        scene->DestroyEntity(entity->GetID());
    }
}

void ProjectileSystem::UpdateProjectile(Entity* entity, Projectile* proj, float deltaTime) {
    // Lifetime kontrolü
    proj->lifetime -= deltaTime;
    if (proj->lifetime <= 0.0f) {
        m_ProjectilesToDestroy.push_back(entity);
        return;
    }

    // Gravity uygula
    if (proj->gravity != 0.0f) {
        proj->velocity.y += proj->gravity * deltaTime;
    }

    // Pozisyonu güncelle
    auto* transform = entity->GetComponent<Transform>();
    if (transform) {
        transform->position += proj->velocity * deltaTime;
    }

    // Collision check (basit, şimdilik sadece zemin)
    if (transform && transform->position.y <= 0.0f) {
        // Zemine çarptı
        proj->hasHit = true;
        m_ProjectilesToDestroy.push_back(entity);
        
        std::cout << "Projectile hit ground at (" 
                  << transform->position.x << ", " 
                  << transform->position.z << ")" << std::endl;
    }
}

bool ProjectileSystem::CheckCollision(Entity* projectile, Scene* scene) {
    // TODO: Implement proper AABB or sphere collision
    // Şimdilik sadece zemin collision var (UpdateProjectile'da)
    return false;
}

Entity* ProjectileSystem::SpawnProjectile(
    Scene* scene,
    const glm::vec3& position,
    const glm::vec3& direction,
    float speed,
    float damage,
    Entity* owner
) {
    if (!scene) return nullptr;

    // Projectile entity oluştur
    Entity* projectile = scene->CreateEntity("Projectile");
    
    // Transform
    auto* transform = projectile->GetComponent<Transform>();
    transform->position = position;
    transform->scale = glm::vec3(0.1f, 0.1f, 0.3f); // Küçük mermi
    
    // Direction'a göre rotation hesapla
    // (Basit: sadece forward direction için)
    glm::vec3 normalizedDir = glm::normalize(direction);
    
    // Projectile component
    auto* proj = projectile->AddComponent<Projectile>();
    proj->velocity = normalizedDir * speed;
    proj->speed = speed;
    proj->damage = damage;
    proj->lifetime = 5.0f;
    proj->gravity = 0.0f; // Bullets genelde gravity etkilenmez (hızlı)
    proj->owner = owner;
    
    // Visual (küçük sarı sphere)
    auto* meshRenderer = projectile->AddComponent<MeshRenderer>();
    
    // Mesh'i resource manager'dan al veya oluştur
    Mesh* bulletMesh = ResourceManager::Get().GetMesh("bullet");
    if (!bulletMesh) {
        bulletMesh = Mesh::CreateSphere(0.5f, 8); // Low-poly sphere
        ResourceManager::Get().AddMesh("bullet", bulletMesh);
    }
    
    meshRenderer->mesh = bulletMesh;
    meshRenderer->color = glm::vec3(1.0f, 1.0f, 0.0f); // Sarı

    std::cout << "Spawned projectile at " << position.x << ", " << position.y << ", " << position.z << std::endl;

    return projectile;
}

} // namespace Archura
