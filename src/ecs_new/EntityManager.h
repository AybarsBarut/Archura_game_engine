#pragma once

#include "Types.h"
#include <queue>
#include <array>
#include <cassert>

namespace Archura::ECS {

class EntityManager {
public:
    EntityManager() {
        for (Entity i = 0; i < MAX_ENTITIES; ++i) {
            m_AvailableEntities.push(i);
        }
    }

    Entity CreateEntity() {
        assert(m_LivingEntityCount < MAX_ENTITIES && "Too many entities in existence.");
        Entity id = m_AvailableEntities.front();
        m_AvailableEntities.pop();
        m_LivingEntityCount++;
        return id;
    }

    void DestroyEntity(Entity entity) {
        assert(entity < MAX_ENTITIES && "Entity out of range.");
        m_Signatures[entity].reset();
        m_AvailableEntities.push(entity);
        m_LivingEntityCount--;
    }

    void SetSignature(Entity entity, Signature signature) {
        assert(entity < MAX_ENTITIES && "Entity out of range.");
        m_Signatures[entity] = signature;
    }

    Signature GetSignature(Entity entity) {
        assert(entity < MAX_ENTITIES && "Entity out of range.");
        return m_Signatures[entity];
    }

private:
    std::queue<Entity> m_AvailableEntities;
    std::array<Signature, MAX_ENTITIES> m_Signatures;
    uint32_t m_LivingEntityCount = 0;
};

} // namespace Archura::ECS
