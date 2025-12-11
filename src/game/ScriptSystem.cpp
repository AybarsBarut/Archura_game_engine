#include "ScriptSystem.h"
#include "DevConsole.h"
#include <iostream>

namespace Archura {

    void ScriptSystem::Init(Scene* scene) {
        m_Scene = scene;
        // std::cout << "Script System Initialized (Mock .NET Host)" << std::endl;
        DevConsole::Get().Log("Script System Initialized");
    }

    void ScriptSystem::Update(float deltaTime) {
        if (!m_Scene) return;

        // Iterate over all entities with ScriptComponent
        for (auto& entityPtr : m_Scene->GetEntities()) {
            Entity* entity = entityPtr.get();
            auto* script = entity->GetComponent<ScriptComponent>();

            if (script) {
                // Mock Execution: In real engine, we would call C# OnUpdate() here
                // For now, we just log occasionally to prove the system is running
                
                // Example: If script name is "Rotator", rotate the entity
                if (script->className == "Rotator") {
                    auto* transform = entity->GetComponent<Transform>();
                    if (transform) {
                        transform->rotation.y += 90.0f * deltaTime;
                    }
                }
            }
        }
    }

    void ScriptSystem::Shutdown() {
        // Unload AppDomain
    }

    void ScriptSystem::ReloadScripts() {
        DevConsole::Get().Log("Reloading Assemblies...");
        // Reload logic
    }

} // namespace Archura
