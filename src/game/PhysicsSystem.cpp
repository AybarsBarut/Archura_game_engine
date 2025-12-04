#include "PhysicsSystem.h"
#include "../ecs/Entity.h"
#include <iostream>

namespace Archura {

    void PhysicsSystem::Init(Scene* scene) {
        m_Scene = scene;
        std::cout << "Physics System Initialized (Internal Solver)" << std::endl;
    }

    void PhysicsSystem::Update(float deltaTime) {
        if (!m_Scene) return;

        Integrate(deltaTime);
        ResolveCollisions();
    }

    void PhysicsSystem::Shutdown() {
        // Cleanup if needed
    }

    void PhysicsSystem::Integrate(float deltaTime) {
        for (auto& entityPtr : m_Scene->GetEntities()) {
            Entity* entity = entityPtr.get();
            auto* rb = entity->GetComponent<RigidBody>();
            auto* transform = entity->GetComponent<Transform>();

            if (rb && transform && !rb->isKinematic) {
                // Apply Gravity
                if (rb->useGravity) {
                    rb->velocity += m_Gravity * deltaTime;
                }

                // Apply Drag
                rb->velocity *= (1.0f - rb->drag * deltaTime);

                // Apply Velocity to Position
                transform->position += rb->velocity * deltaTime;
            }
        }
    }

    void PhysicsSystem::ResolveCollisions() {
        // Very basic O(N^2) AABB collision resolution
        // In a real engine, use a spatial partition (Octree/BVH) and PhysX
        
        auto& entities = m_Scene->GetEntities();
        for (size_t i = 0; i < entities.size(); ++i) {
            Entity* entityA = entities[i].get();
            auto* rbA = entityA->GetComponent<RigidBody>();
            auto* colA = entityA->GetComponent<BoxCollider>();
            auto* transA = entityA->GetComponent<Transform>();

            if (!rbA || !colA || !transA || rbA->isKinematic) continue;

            for (size_t j = 0; j < entities.size(); ++j) {
                if (i == j) continue;

                Entity* entityB = entities[j].get();
                auto* colB = entityB->GetComponent<BoxCollider>();
                auto* transB = entityB->GetComponent<Transform>();

                // We collide with static objects (no RB) or dynamic objects (RB)
                if (!colB || !transB) continue;

                // Check AABB
                if (CheckAABB(transA->position, colA->size * transA->scale, transB->position, colB->size * transB->scale)) {
                    // Collision Detected!
                    // Very simple resolution: Stop velocity and push out (naive)
                    
                    // Determine overlap on Y axis to see if it's ground
                    float yOverlap = (colA->size.y * transA->scale.y * 0.5f + colB->size.y * transB->scale.y * 0.5f) - std::abs(transA->position.y - transB->position.y);
                    
                    if (yOverlap > 0) {
                        // If we are falling and hitting something below
                        if (rbA->velocity.y < 0 && transA->position.y > transB->position.y) {
                            transA->position.y += yOverlap;
                            rbA->velocity.y = 0;
                        }
                    }
                }
            }
        }
    }

    bool PhysicsSystem::CheckAABB(const glm::vec3& posA, const glm::vec3& sizeA, const glm::vec3& posB, const glm::vec3& sizeB) {
        glm::vec3 halfA = sizeA * 0.5f;
        glm::vec3 halfB = sizeB * 0.5f;

        bool xOverlap = std::abs(posA.x - posB.x) <= (halfA.x + halfB.x);
        bool yOverlap = std::abs(posA.y - posB.y) <= (halfA.y + halfB.y);
        bool zOverlap = std::abs(posA.z - posB.z) <= (halfA.z + halfB.z);

        return xOverlap && yOverlap && zOverlap;
    }

    bool PhysicsSystem::Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Entity** outEntity, glm::vec3* outHitPoint) {
        // Re-implementing the raycast logic from main.cpp here for centralization
        // ... (Simplified for brevity, can be copied from main.cpp later)
        return false; 
    }

} // namespace Archura
