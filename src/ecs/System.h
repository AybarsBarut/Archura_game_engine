#pragma once

namespace Archura {

class Scene;

/**
 * @brief System base class - Game logic sistemleri buradan türer
 */
class System {
public:
    virtual ~System() = default;

    virtual void Init(Scene* scene) { m_Scene = scene; }
    virtual void Update(float deltaTime) = 0;
    virtual void Shutdown() {}

protected:
    Scene* m_Scene = nullptr;
};

} // namespace Archura
