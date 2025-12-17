#include "ParticleSystem.h"
#include "Particle.h"
#include "../ecs/Entity.h"
#include "../rendering/Mesh.h"
#include <random>

namespace Archura {

void ParticleSystem::Update(float deltaTime) {
    if (!m_Scene) return;

    std::vector<Entity*> deadParticles;

    for (auto& entityPtr : m_Scene->GetEntities()) {
        auto* particle = entityPtr->GetComponent<Particle>();
        if (!particle) continue;

        auto* transform = entityPtr->GetComponent<Transform>();
        auto* meshRenderer = entityPtr->GetComponent<MeshRenderer>();

        // Life cycle
        particle->lifetime -= deltaTime;
        if (particle->lifetime <= 0) {
            deadParticles.push_back(entityPtr.get());
            continue;
        }

        // Physics
        particle->velocity += particle->acceleration * deltaTime;
        if (transform) {
            transform->position += particle->velocity * deltaTime;
        }

        // Visualization (Fade out)
        if (meshRenderer) {
            float alpha = particle->lifetime / particle->startLifetime;
            meshRenderer->color = glm::vec3(particle->color) * alpha; // Simple fade
            // Note: Real transparency requires alpha blending setup in RenderSystem
        }
    }

    for (auto* p : deadParticles) {
        m_Scene->DestroyEntity(p->GetID());
    }
}

void ParticleSystem::EmitBurst(Scene* scene, const glm::vec3& position, const glm::vec3& normal, int count, glm::vec4 color, float speed, float size, float lifetime, bool useGravity) {
    if (!scene) return;
    
    // Seed for randomness
    static std::mt19937 mt(std::random_device{}());
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    for (int i = 0; i < count; i++) {
        Entity* p = scene->CreateEntity("Particle");
        
        auto* transform = p->AddComponent<Transform>();
        transform->position = position;
        transform->scale = glm::vec3(size);

        auto* particle = p->AddComponent<Particle>();
        particle->color = color;
        particle->size = size;
        particle->lifetime = lifetime; // + random variance?
        particle->startLifetime = lifetime;
        
        // Random Direction mostly along normal
        glm::vec3 rDir = glm::vec3(dist(mt), dist(mt), dist(mt));
        if (glm::dot(rDir, normal) < 0) rDir = -rDir; // reflect to front
        glm::vec3 finalDir = glm::normalize(normal + rDir); 

        particle->velocity = finalDir * speed * (0.5f + (dist(mt) + 1.0f) * 0.5f); // vary speed
        
        if (useGravity) {
            particle->acceleration = glm::vec3(0.0f, -9.81f, 0.0f);
        }

        auto* mr = p->AddComponent<MeshRenderer>();
        // Using Cube for now as "pixel" particle
        static Mesh* particleMesh = Mesh::CreateCube(1.0f); 
        mr->mesh = particleMesh;
        mr->color = glm::vec3(color);
    }
}

} // namespace Archura
