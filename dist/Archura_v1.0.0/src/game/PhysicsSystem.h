#pragma once

#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include <vector>
#include <glm/glm.hpp>

namespace Archura {

    class Scene;

    class PhysicsSystem {
    public:
        void Init(Scene* scene);
        void Update(float deltaTime);
        void Shutdown();

        // Raycast support (Basic AABB)
        bool Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, Entity** outEntity, glm::vec3* outHitPoint);

    private:
        Scene* m_Scene = nullptr;
        glm::vec3 m_Gravity = glm::vec3(0.0f, -9.81f, 0.0f);

        void Integrate(float deltaTime);
        void ResolveCollisions();
        bool CheckAABB(const glm::vec3& posA, const glm::vec3& sizeA, const glm::vec3& posB, const glm::vec3& sizeB);
    };

} // namespace Archura
