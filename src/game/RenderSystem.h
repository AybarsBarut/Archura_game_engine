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

    void DrawColliders();

    void SetCamera(Camera* camera) { m_Camera = camera; }
    Camera* GetCamera() const { return m_Camera; }

private:
    Camera* m_Camera;
    std::unique_ptr<Shader> m_DefaultShader;
    class Mesh* m_DebugMesh = nullptr;
    
    // Lighting
    // Lighting
    // Dynamic lighting handling in Update()

    // Shadows
    void InitShadowMap();
    unsigned int m_DepthMapFBO = 0;
    unsigned int m_DepthMapTexture = 0;
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    std::unique_ptr<Shader> m_DepthShader;
    glm::mat4 m_LightSpaceMatrix;
};

} // namespace Archura
