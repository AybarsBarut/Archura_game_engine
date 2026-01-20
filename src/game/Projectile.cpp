#include "Projectile.h"
#include "ProjectileSystem.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include "../rendering/Mesh.h"
#include "../core/ResourceManager.h"
#include "Lifetime.h"
#include "SurfaceProperty.h"
#include "Particle.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <iostream>
#include <random>
#include <algorithm>

namespace Archura {

void ProjectileSystem::Update(float deltaTime) {
    if (!m_Scene) return;

    m_ProjectilesToDestroy.clear();
    m_ProjectilesToDestroy.reserve(10); // Pre-allocate for common case

    // Tum mermileri guncelle
    for (auto& entityPtr : m_Scene->GetEntities()) {
        auto* projectile = entityPtr->GetComponent<Projectile>();
        if (projectile) {
            UpdateProjectile(entityPtr.get(), projectile, deltaTime);
            
            // Diger varliklarla carpismayi kontrol et
            if (CheckCollision(entityPtr.get(), m_Scene)) {
                projectile->hasHit = true;
                m_ProjectilesToDestroy.push_back(entityPtr.get());
            }
        }
    }

    // Vurmus veya suresi dolmus mermileri yok et
    for (auto* entity : m_ProjectilesToDestroy) {
        m_Scene->DestroyEntity(entity->GetID());
    }

    // Generic Lifecycle System (Simple implementation here for now)
    std::vector<Entity*> expiredEntities;
    for (auto& entityPtr : m_Scene->GetEntities()) {
        auto* lifetime = entityPtr->GetComponent<Lifetime>();
        if (lifetime) {
            lifetime->remainingTime -= deltaTime;
            if (lifetime->remainingTime <= 0.0f) {
                expiredEntities.push_back(entityPtr.get());
            }
        }
    }
    for (auto* entity : expiredEntities) {
        m_Scene->DestroyEntity(entity->GetID());
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

    bool hasHitAny = false;

    for (const auto& targetPtr : scene->GetEntities()) {
        Entity* target = targetPtr.get();
        
        // Kendine carpma
        if (target == proj->owner) continue;
        if (target == projectile) continue;

        auto* collider = target->GetComponent<BoxCollider>();
        auto* transform = target->GetComponent<Transform>();
        auto* health = target->GetComponent<Health>();

        if (collider && transform) {
            // Hedef AABB
            glm::vec3 halfSize = collider->size * transform->scale * 0.5f;
            glm::vec3 targetMin = transform->position - halfSize;
            glm::vec3 targetMax = transform->position + halfSize;

            // AABB vs AABB Collision Detection
            bool collisionX = projMax.x >= targetMin.x && projMin.x <= targetMax.x;
            bool collisionY = projMax.y >= targetMin.y && projMin.y <= targetMax.y;
            bool collisionZ = projMax.z >= targetMin.z && projMin.z <= targetMax.z;

            if (collisionX && collisionY && collisionZ) {
                
                // Calculate Hit Normal (Axis of least penetration)
                float overlapX = std::min(projMax.x, targetMax.x) - std::max(projMin.x, targetMin.x);
                float overlapY = std::min(projMax.y, targetMax.y) - std::max(projMin.y, targetMin.y);
                float overlapZ = std::min(projMax.z, targetMax.z) - std::max(projMin.z, targetMin.z);

                glm::vec3 normal(0.0f);
                glm::vec3 hitPos = projTransform->position;

                if (overlapX < overlapY && overlapX < overlapZ) {
                    normal = (projTransform->position.x > transform->position.x) ? glm::vec3(1, 0, 0) : glm::vec3(-1, 0, 0);
                    hitPos.x = (normal.x > 0) ? targetMax.x : targetMin.x; // Snap to surface
                } else if (overlapY < overlapX && overlapY < overlapZ) {
                    normal = (projTransform->position.y > transform->position.y) ? glm::vec3(0, 1, 0) : glm::vec3(0, -1, 0);
                    hitPos.y = (normal.y > 0) ? targetMax.y : targetMin.y;
                } else {
                    normal = (projTransform->position.z > transform->position.z) ? glm::vec3(0, 0, 1) : glm::vec3(0, 0, -1);
                    hitPos.z = (normal.z > 0) ? targetMax.z : targetMin.z;
                }

                // Check Surface Property
                SurfaceType surfaceType = SurfaceType::Concrete; // Default
                if (auto* surfaceProp = target->GetComponent<SurfaceProperty>()) {
                    surfaceType = surfaceProp->type;
                }

                // Spawn Decal with Surface Type
                SpawnDecal(scene, hitPos, normal, surfaceType);

                // Apply Damage if applicable
                if (health) {
                    health->current -= proj->damage;
                    if (health->current < 0) health->current = 0;
                }
                
                hasHitAny = true;
                // std::cout << "Hit Entity " << target->GetName() << "! Damage: " << proj->damage << " Health: " << (health ? std::to_string(health->current) : "N/A") << std::endl;
                
                // Don't return strictly true immediately if we want to pierce, but for now destroy on first hit
                return true;
            }
        }
    }
    return false;
}

void ProjectileSystem::SpawnDecal(Scene* scene, const glm::vec3& position, const glm::vec3& normal, SurfaceType surfaceType) {
    if (!scene) return;

    Entity* decal = scene->CreateEntity("Decal");
    
    // Add Lifetime
    decal->AddComponent<Lifetime>(10.0f); // 10 seconds lifetime

    // Transform
    auto* transform = decal->GetComponent<Transform>();
    transform->position = position + normal * 0.02f; // Slight offset to prevent Z-fighting
    transform->scale = glm::vec3(0.2f); // 20cm decal

    // Orientation logic (Align Quad +Y to Normal)
    // Simple axis aligned handling for Euler angles
    if (glm::abs(normal.y) > 0.9f) {
        // Ceiling or Floor
        if (normal.y > 0) transform->rotation = glm::vec3(0.0f, 0.0f, 0.0f); // Up
        else transform->rotation = glm::vec3(180.0f, 0.0f, 0.0f); // Down
    } else if (glm::abs(normal.x) > 0.9f) {
        // Walls X
        if (normal.x > 0) transform->rotation = glm::vec3(0.0f, 0.0f, -90.0f);
        else transform->rotation = glm::vec3(0.0f, 0.0f, 90.0f); 
    } else {
        // Walls Z
        if (normal.z > 0) transform->rotation = glm::vec3(90.0f, 0.0f, 0.0f);
        else transform->rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
    }

    // Mesh Renderer
    auto* meshRenderer = decal->AddComponent<MeshRenderer>();
    
    // Use Plane mesh
    static Mesh* decalMesh = Mesh::CreatePlane(1.0f, 1.0f); // Static to avoid recreating
    meshRenderer->mesh = decalMesh;

    // Use Bullet Hole Texture if available, else black color
    // Color based on Surface Type
    glm::vec3 decalColor = glm::vec3(0.1f); // Default Dark Grey (Concrete)
    
    switch (surfaceType) {
        case SurfaceType::Flesh:
             decalColor = glm::vec3(0.8f, 0.0f, 0.0f); // Red Blood
             break;
        case SurfaceType::Wood:
             decalColor = glm::vec3(0.6f, 0.4f, 0.2f); // Brown Wood
             break;
        case SurfaceType::Metal:
             decalColor = glm::vec3(0.7f, 0.7f, 0.75f); // Bluish Grey Metal
             break;
        case SurfaceType::Dirt:
             decalColor = glm::vec3(0.4f, 0.3f, 0.2f); // Dark Brown Dirt
             break;
        case SurfaceType::Glass:
             decalColor = glm::vec3(0.5f, 0.9f, 0.9f); // Cyan Glass
             break;
        default:
             break;
    }
    
    // Randomize slightly for variety (optional, keep simple for now)
    
    // Texture logic placeholder (eventually use different textures per material)
    meshRenderer->color = decalColor;

    // --- Spawn Particles ---
    int particleCount = 5;
    float particleSpeed = 2.0f;
    bool particleGravity = true;
    float particleSize = 0.05f;

    if (surfaceType == SurfaceType::Metal) {
        particleCount = 10;
        particleSpeed = 5.0f;
        particleGravity = true;
        decalColor = glm::vec4(1.0f, 0.9f, 0.5f, 1.0f); // Sparks are bright
    } else if (surfaceType == SurfaceType::Flesh) {
        particleCount = 8;
        particleSpeed = 1.0f;
        particleGravity = true;
    } else if (surfaceType == SurfaceType::Concrete) {
        particleSpeed = 1.5f;
    }

    static std::mt19937 mt(std::random_device{}());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for(int i=0; i<particleCount; ++i) {
        Entity* p = scene->CreateEntity("Particle");
        auto* pt = p->AddComponent<Transform>();
        pt->position = position + normal * 0.1f;
        pt->scale = glm::vec3(particleSize);

        auto* par = p->AddComponent<Particle>();
        par->color = glm::vec4(decalColor, 1.0f);
        par->lifetime = 0.5f + (dist(mt) + 1.0f) * 0.2f;
        par->startLifetime = par->lifetime;

        glm::vec3 rDir = glm::vec3(dist(mt), dist(mt), dist(mt));
        if(glm::dot(rDir, normal) < 0) rDir = -rDir;
        par->velocity = glm::normalize(normal + rDir) * particleSpeed;
        if(particleGravity) par->acceleration = glm::vec3(0.0f, -9.81f, 0.0f);

        auto* pmr = p->AddComponent<MeshRenderer>();
        static Mesh* cubeMesh = Mesh::CreateCube(1.0f); // Cube pixel
        pmr->mesh = cubeMesh;
        pmr->color = glm::vec3(decalColor);
    }
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
