#pragma once

#include <string>
#include <memory>
#include <glm/glm.hpp>

namespace Archura {

class Shader;
class Texture;

/**
 * @brief HUD Renderer - 2D overlay rendering (crosshair, ammo, health, etc.)
 */
class HUDRenderer {
public:
    HUDRenderer();
    ~HUDRenderer();

    bool Init();
    void Shutdown();

    // HUD rendering
    void BeginHUD(float width, float height);
    void EndHUD();

    // Primitive shapes
    void DrawRect(float x, float y, float width, float height, const glm::vec4& color);
    void DrawTexture(Texture* texture, float x, float y, float width, float height);
    
    // HUD elements
    void DrawCrosshair(float size = 20.0f, const glm::vec4& color = glm::vec4(1.0f));
    void DrawAmmoCounter(int current, int total, float x, float y);
    void DrawHealthBar(float health, float maxHealth, float x, float y, float width, float height);

    void SetScreenSize(float width, float height);

private:
    void CreateQuadMesh();

private:
    std::unique_ptr<Shader> m_HUDShader;
    unsigned int m_QuadVAO, m_QuadVBO, m_QuadEBO;
    float m_ScreenWidth, m_ScreenHeight;
};

} // namespace Archura
