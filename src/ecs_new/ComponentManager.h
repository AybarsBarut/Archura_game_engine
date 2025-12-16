#pragma once

#include "Types.h"
#include "ComponentArray.h"
#include <unordered_map>
#include <memory>
#include <typeindex>
#include <cassert>

namespace Archura::ECS {

class ComponentManager {
public:
    template<typename T>
    void RegisterComponent() {
        assert(m_ComponentTypes.find(typeid(T)) == m_ComponentTypes.end() && "Registering component type more than once.");

        m_ComponentTypes[typeid(T)] = m_NextComponentType;
        m_ComponentArrays[typeid(T)] = std::make_shared<ComponentArray<T>>();
        
        m_NextComponentType++;
    }

    template<typename T>
    ComponentType GetComponentType() {
        assert(m_ComponentTypes.find(typeid(T)) != m_ComponentTypes.end() && "Component not registered before use.");
        return m_ComponentTypes[typeid(T)];
    }

    template<typename T>
    void AddComponent(Entity entity, T component) {
        GetComponentArray<T>()->InsertData(entity, component);
    }

    template<typename T>
    void RemoveComponent(Entity entity) {
        GetComponentArray<T>()->RemoveData(entity);
    }

    template<typename T>
    T& GetComponent(Entity entity) {
        return GetComponentArray<T>()->GetData(entity);
    }

    void EntityDestroyed(Entity entity) {
        for (auto const& pair : m_ComponentArrays) {
            auto const& component = pair.second;
            component->EntityDestroyed(entity);
        }
    }

private:
    template<typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        assert(m_ComponentTypes.find(typeid(T)) != m_ComponentTypes.end() && "Component not registered before use.");
        return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeid(T)]);
    }

    std::unordered_map<std::type_index, ComponentType> m_ComponentTypes;
    std::unordered_map<std::type_index, std::shared_ptr<IComponentArray>> m_ComponentArrays;
    ComponentType m_NextComponentType = 0;
};

} // namespace Archura::ECS
