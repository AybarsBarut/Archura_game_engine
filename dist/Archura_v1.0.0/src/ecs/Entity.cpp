#include "Entity.h"
#include <algorithm>

namespace Archura {

Entity::Entity(EntityID id, const std::string& name)
    : m_ID(id), m_Name(name)
{
    // Her varlik otomatik olarak Donusum bilesenine sahip
    AddComponent<Transform>();
}

// ==================== Scene ====================

Scene::Scene(const std::string& name)
    : m_Name(name)
{
}

Entity* Scene::CreateEntity(const std::string& name) {
    auto entity = std::make_shared<Entity>(m_NextEntityID++, name);
    m_Entities.push_back(entity);
    return entity.get();
}

void Scene::DestroyEntity(EntityID id) {
    auto it = std::remove_if(m_Entities.begin(), m_Entities.end(),
        [id](const std::shared_ptr<Entity>& entity) {
            return entity->GetID() == id;
        });
    m_Entities.erase(it, m_Entities.end());
}

Entity* Scene::GetEntity(EntityID id) {
    for (auto& entity : m_Entities) {
        if (entity->GetID() == id) {
            return entity.get();
        }
    }
    return nullptr;
}

} // namespace Archura
