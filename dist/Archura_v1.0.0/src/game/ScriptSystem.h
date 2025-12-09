#pragma once

#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include <string>
#include <vector>

namespace Archura {

    class Scene;

    class ScriptSystem {
    public:
        void Init(Scene* scene);
        void Update(float deltaTime);
        void Shutdown();

        void ReloadScripts();

    private:
        Scene* m_Scene = nullptr;
        
        // In a real implementation, this would hold the Mono Domain / Assembly
        // void* m_MonoDomain; 
    };

} // namespace Archura
