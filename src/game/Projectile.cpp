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

    // Tum mermileri guncelle
    for (auto& entityPtr : scene->GetEntities()) {
        auto* projectile = entityPtr->GetComponent<Projectile>();
        if (projectile) {
            UpdateProjectile(entityPtr.get(), projectile, deltaTime);
            
            // Diger varliklarla carpismayi kontrol et
            if (CheckCollision(entityPtr.get(), scene)) {
                projectile->hasHit = true;
                m_ProjectilesToDestroy.push_back(entityPtr.get());
            }
        }
    }

    // Vurmus veya suresi dolmus mermileri yok et
    for (auto* entity : m_ProjectilesToDestroy) {
        scene->DestroyEntity(entity->GetID());
    }
}

void ProjectileSystem::UpdateProjectile(Entity* entity, Projectile* proj, float deltaTime) {
    auto* transform = entity->GetComponent<Transform>();
    if (!transform) return;

    // Omur suresi kontrolu
    proj->lifetime -= deltaTime;
    if (proj->lifetime <= 0.0f) {
        m_ProjectilesToDestroy.push_back(entity);
        return;
    }

    // El Bombasi Fitili
    if (proj->type == Projectile::ProjectileType::Grenade) {
        proj->fuseTimer -= deltaTime;
        if (proj->fuseTimer <= 0.0f) {
            // Patla!
            // std::cout << "BOOM! Grenade exploded." << std::endl;
            // Alan hasari mantigi burada (basitlestirilmis: sadece yok et)
            // Gercek bir uygulamada, tum varliklara olan mesafeyi kontrol ederdik
            m_ProjectilesToDestroy.push_back(entity);
            return;
        }
    }

    // Yercekimi
    if (proj->gravity != 0.0f) {
        proj->velocity.y += proj->gravity * deltaTime;
    }

    // Hareket
    glm::vec3 nextPos = transform->position + proj->velocity * deltaTime;

    // Zemin Carpismasi (Basit)
    if (nextPos.y < 0.2f) { // Zemin seviyesi
        if (proj->type == Projectile::ProjectileType::Grenade) {
            // Sekme
            nextPos.y = 0.2f;
            proj->velocity.y = -proj->velocity.y * 0.5f; // Enerji kaybi
            proj->velocity.x *= 0.8f; // Surtunme
            proj->velocity.z *= 0.8f;
            
            // Cok yavassa dur
            if (glm::length(proj->velocity) < 0.5f) {
                proj->velocity = glm::vec3(0.0f);
            }
        } else {
            // Mermiler zemine carpinca yok olur
            m_ProjectilesToDestroy.push_back(entity);
            return;
        }
    }

    transform->position = nextPos;
}

bool ProjectileSystem::CheckCollision(Entity* projectile, Scene* scene) {
    auto* proj = projectile->GetComponent<Projectile>();
    auto* projTransform = projectile->GetComponent<Transform>();
    
    if (!proj || !projTransform) return false;

    // Mermi AABB (kucuk bir kutu)
    glm::vec3 projMin = projTransform->position - glm::vec3(0.1f);
    glm::vec3 projMax = projTransform->position + glm::vec3(0.1f);

    for (const auto& targetPtr : scene->GetEntities()) {
        Entity* target = targetPtr.get();
        
        // Kendine carpma
        if (target == proj->owner) continue;
        if (target == projectile) continue;

        auto* collider = target->GetComponent<BoxCollider>();
        auto* transform = target->GetComponent<Transform>();
        auto* health = target->GetComponent<Health>();

        if (collider && transform && health) {
            // Hedef AABB
            glm::vec3 halfSize = collider->size * transform->scale * 0.5f;
            glm::vec3 targetMin = transform->position - halfSize;
            glm::vec3 targetMax = transform->position + halfSize;

            // AABB vs AABB
            bool collisionX = projMax.x >= targetMin.x && projMin.x <= targetMax.x;
            bool collisionY = projMax.y >= targetMin.y && projMin.y <= targetMax.y;
            bool collisionZ = projMax.z >= targetMin.z && projMin.z <= targetMax.z;

            if (collisionX && collisionY && collisionZ) {
                // Vurus!
                health->current -= proj->damage;
                if (health->current < 0) health->current = 0;
                
                // std::cout << "Hit Entity " << target->GetName() << "! Damage: " << proj->damage << " Health: " << health->current << std::endl;
                return true;
            }
        }
    }
    return false;
}

Entity* ProjectileSystem::SpawnProjectile(
    Scene* scene,
    const glm::vec3& position,
    const glm::vec3& direction,
    float speed,
    float damage,
    Entity* owner,
    Projectile::ProjectileType type
) {
    if (!scene) return nullptr;

    // Mermi varligi olustur
    Entity* projectile = scene->CreateEntity("Projectile");
    
    // Donusum
    auto* transform = projectile->GetComponent<Transform>();
    transform->position = position;
    
    auto* meshRenderer = projectile->AddComponent<MeshRenderer>();
    
    if (type == Projectile::ProjectileType::Grenade) {
        meshRenderer->mesh = Mesh::CreateCube(1.0f); 
        meshRenderer->color = glm::vec3(0.0f, 0.5f, 0.0f); // Yesil El Bombasi
        transform->scale = glm::vec3(0.3f);
    } else {
        // Mermi
        Mesh* bulletMesh = ResourceManager::Get().GetMesh("bullet");
        if (!bulletMesh) {
            bulletMesh = Mesh::CreateSphere(0.5f, 8);
            ResourceManager::Get().AddMesh("bullet", bulletMesh);
        }
        meshRenderer->mesh = bulletMesh;
        meshRenderer->color = glm::vec3(1.0f, 1.0f, 0.0f); // Sari Mermi
        transform->scale = glm::vec3(0.1f, 0.1f, 0.3f);
    }

    // Yone gore rotasyon hesapla (Basit)
    glm::vec3 normalizedDir = glm::normalize(direction);
    
    // Mermi bileseni
    auto* proj = projectile->AddComponent<Projectile>();
    proj->velocity = normalizedDir * speed;
    proj->speed = speed;
    proj->damage = damage;
    proj->owner = owner;
    proj->type = type;
    
    if (type == Projectile::ProjectileType::Grenade) {
        proj->gravity = -9.81f;
        proj->lifetime = 10.0f;
        proj->fuseTimer = 5.0f;
    } else {
        proj->gravity = 0.0f; // Mermiler duz ucar
        proj->lifetime = 5.0f;
    }

    // std::cout << "Spawned projectile at " << position.x << ", " << position.y << ", " << position.z << std::endl;

    return projectile;
}

} // namespace Archura
