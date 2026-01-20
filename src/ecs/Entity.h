#pragma once

#include "Component.h"
#include <string>
#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace Archura {

using EntityID = uint32_t;

/**
 * @brief Entity sınıfı - Basit ECS implementasyonu
 * 
 * Her entity bir ID'ye ve component'lere sahiptir
 */
class Entity {
public:
    Entity(EntityID id, const std::string& name = "Entity");
    ~Entity() = default;

    EntityID GetID() const { return m_ID; }
    const std::string& GetName() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }

    // Component yönetimi
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args) {
        std::type_index typeIndex = typeid(T);
        
        // Zaten varsa eski component'i döndür
        if (m_Components.find(typeIndex) != m_Components.end()) {
            return static_cast<T*>(m_Components[typeIndex].get());
        }
        
        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        m_Components[typeIndex] = component;
        return component.get();
    }

    template<typename T>
    T* GetComponent() {
        std::type_index typeIndex = typeid(T);
        auto it = m_Components.find(typeIndex);
        if (it != m_Components.end()) {
            return static_cast<T*>(it->second.get());
        }
        return nullptr;
    }

    template<typename T>
    bool HasComponent() {
        std::type_index typeIndex = typeid(T);
        return m_Components.find(typeIndex) != m_Components.end();
    }

    template<typename T>
    void RemoveComponent() {
        std::type_index typeIndex = typeid(T);
        m_Components.erase(typeIndex);
    }

private:
    EntityID m_ID;
    std::string m_Name;
    std::unordered_map<std::type_index, std::shared_ptr<Component>> m_Components;
};

/**
 * @brief Scene sınıfı - Entity'leri yönetir
 */
class Scene {
public:
    Scene(const std::string& name = "Default Scene");
    ~Scene() = default;

    Entity* CreateEntity(const std::string& name = "Entity");
    void DestroyEntity(EntityID id);
    Entity* GetEntity(EntityID id);
    
    const std::vector<std::shared_ptr<Entity>>& GetEntities() const { return m_Entities; }

private:
    std::string m_Name;
    EntityID m_NextEntityID = 1;
    std::vector<std::shared_ptr<Entity>> m_Entities;
};

} // namespace Archura
