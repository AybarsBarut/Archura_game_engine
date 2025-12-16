#pragma once

#include "ComponentManager.h"
#include "EntityManager.h"
#include "System.h"
#include "Types.h"
#include <memory>
#include <unordered_map>
#include <typeindex>

namespace Archura::ECS {

class Coordinator {
public:
    void Init() {
        m_EntityManager = std::make_unique<EntityManager>();
        m_ComponentManager = std::make_unique<ComponentManager>();
    }

    // Entity methods
    Entity CreateEntity() {
        return m_EntityManager->CreateEntity();
    }

    void DestroyEntity(Entity entity) {
        m_EntityManager->DestroyEntity(entity);
        m_ComponentManager->EntityDestroyed(entity);
        m_SystemManager->EntityDestroyed(entity);
    }

    // Component methods
    template<typename T>
    void RegisterComponent() {
        m_ComponentManager->RegisterComponent<T>();
    }

    template<typename T>
    void AddComponent(Entity entity, T component) {
        m_ComponentManager->AddComponent<T>(entity, component);

        auto signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentType<T>(), true);
        m_EntityManager->SetSignature(entity, signature);
        
        m_SystemManager->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    void RemoveComponent(Entity entity) {
        m_ComponentManager->RemoveComponent<T>(entity);

        auto signature = m_EntityManager->GetSignature(entity);
        signature.set(m_ComponentManager->GetComponentType<T>(), false);
        m_EntityManager->SetSignature(entity, signature);

        m_SystemManager->EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    T& GetComponent(Entity entity) {
        return m_ComponentManager->GetComponent<T>(entity);
    }

    template<typename T>
    ComponentType GetComponentType() {
        return m_ComponentManager->GetComponentType<T>();
    }

    // System methods
    template<typename T>
    std::shared_ptr<T> RegisterSystem() {
        return m_SystemManager->RegisterSystem<T>();
    }

    template<typename T>
    void SetSystemSignature(Signature signature) {
        m_SystemManager->SetSignature<T>(signature);
    }

private:
    // Helper class for SystemManager inside Coordinator to keep header clean
    class SystemManager {
    public:
        template<typename T>
        std::shared_ptr<T> RegisterSystem() {
            auto system = std::make_shared<T>();
            m_Systems.insert({typeid(T), system});
            return system;
        }

        template<typename T>
        void SetSignature(Signature signature) {
            m_Signatures.insert({typeid(T), signature});
        }

        void EntityDestroyed(Entity entity) {
            for (auto const& pair : m_Systems) {
                auto const& system = pair.second;
                system->m_Entities.erase(entity);
            }
        }

        void EntitySignatureChanged(Entity entity, Signature entitySignature) {
            for (auto const& pair : m_Systems) {
                auto const& type = pair.first;
                auto const& system = pair.second;
                auto const& systemSignature = m_Signatures[type];

                if ((entitySignature & systemSignature) == systemSignature) {
                    system->m_Entities.insert(entity);
                } else {
                    system->m_Entities.erase(entity);
                }
            }
        }

    private:
        std::unordered_map<std::type_index, Signature> m_Signatures;
        std::unordered_map<std::type_index, std::shared_ptr<System>> m_Systems;
    };

    std::unique_ptr<EntityManager> m_EntityManager;
    std::unique_ptr<ComponentManager> m_ComponentManager;
    std::unique_ptr<SystemManager> m_SystemManager = std::make_unique<SystemManager>();
};

} // namespace Archura::ECS
