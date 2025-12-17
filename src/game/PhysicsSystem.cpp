#include "PhysicsSystem.h"
#include "../ecs/Entity.h"
#include <iostream>

namespace Archura {

    void PhysicsSystem::Init(Scene* scene) {
        m_Scene = scene;
        // std::cout << "Physics System Initialized (Internal Solver)" << std::endl;
    }

    void PhysicsSystem::Update(float deltaTime) {
        if (!m_Scene) return;

        Integrate(deltaTime);
        ResolveCollisions();
    }

    void PhysicsSystem::Shutdown() {
        // Gerekirse temizlik
    }

    void PhysicsSystem::Integrate(float deltaTime) {
        for (auto& entityPtr : m_Scene->GetEntities()) {
            Entity* entity = entityPtr.get();
            auto* rb = entity->GetComponent<RigidBody>();
            auto* transform = entity->GetComponent<Transform>();

            if (rb && transform && !rb->isKinematic) {
                // Yerçekimi Uygula
                if (rb->useGravity) {
                    rb->velocity += m_Gravity * deltaTime;
                }

                // Sürüklemeyi (Direnç) Uygula
                rb->velocity *= (1.0f - rb->drag * deltaTime);

                // Hızı Pozisyona Uygula
                transform->position += rb->velocity * deltaTime;
            }
        }
    }

    void PhysicsSystem::ResolveCollisions() {
        // Çok temel O(N^2) AABB çarpışma çözümü
        // Gerçek bir motorda, uzaysal bölümleme (Octree/BVH) ve PhysX kullanın
        
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

                // Statik nesnelerle (RB yok) veya dinamik nesnelerle (RB) çarpışıyoruz
                if (!colB || !transB) continue;

                // Tetikleyicileri (Trigger) Kontrol Et
                if (colA->isTrigger || colB->isTrigger) continue;

                // AABB Kontrolü
                if (CheckAABB(transA->position, colA->size * transA->scale, transB->position, colB->size * transB->scale)) {
                    // Çarpışma Tespit Edildi!
                    // Çok basit çözüm: Hızı durdur ve dışarı it (saf yöntem)
                    
                    // Zemin olup olmadığını anlamak için Y eksenindeki örtüşmeyi belirle
                    float yOverlap = (colA->size.y * transA->scale.y * 0.5f + colB->size.y * transB->scale.y * 0.5f) - std::abs(transA->position.y - transB->position.y);
                    
                    if (yOverlap > 0) {
                        // Eğer düşüyorsak ve aşağıda bir şeye çarpıyorsak
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
        // Merkezileştirme için main.cpp'den ışın izleme (raycast) mantığını burada yeniden uyguluyoruz
        // ... (Kısalık için basitleştirildi, daha sonra main.cpp'den kopyalanabilir)
        return false; 
    }

} // namespace Archura
