#pragma once

#include "../ecs/System.h"
#include "../rendering/Camera.h"
#include "../rendering/Shader.h"
#include <memory>

namespace Archura {

/**
 * @brief Rendering System - Tüm entity'leri render eder
 */
class RenderSystem : public System {
public:
    RenderSystem(Camera* camera);
    ~RenderSystem() override;

    void Init(Scene* scene) override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    void SetCamera(Camera* camera) { m_Camera = camera; }
    Camera* GetCamera() const { return m_Camera; }

private:
    Camera* m_Camera;
    std::unique_ptr<Shader> m_DefaultShader;
    
    // Lighting
    glm::vec3 m_LightPos = glm::vec3(5.0f, 10.0f, 5.0f);
    glm::vec3 m_LightColor = glm::vec3(1.0f);
};

} // namespace Archura
