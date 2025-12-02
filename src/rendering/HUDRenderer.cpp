#include "HUDRenderer.h"
#include "Shader.h"
#include "Texture.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace Archura {

HUDRenderer::HUDRenderer()
    : m_HUDShader(nullptr)
    , m_QuadVAO(0)
    , m_QuadVBO(0)
    , m_QuadEBO(0)
    , m_ScreenWidth(1920.0f)
    , m_ScreenHeight(1080.0f)
{
}

HUDRenderer::~HUDRenderer() {
    Shutdown();
}

bool HUDRenderer::Init() {
    // HUD shader (basit 2D shader)
    m_HUDShader = new Shader();
    
    std::string vertexSrc = R"(
        #version 330 core
        layout (location = 0) in vec2 aPos;
        layout (location = 1) in vec2 aTexCoords;
        
        out vec2 TexCoords;
        
        uniform mat4 uProjection;
        uniform mat4 uModel;
        
        void main() {
            TexCoords = aTexCoords;
            gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
        }
    )";
    
    std::string fragmentSrc = R"(
        #version 330 core
        in vec2 TexCoords;
        out vec4 FragColor;
        
        uniform vec4 uColor;
        uniform sampler2D uTexture;
        uniform bool uUseTexture;
        
        void main() {
            if (uUseTexture) {
                FragColor = texture(uTexture, TexCoords) * uColor;
            } else {
                FragColor = uColor;
            }
        }
    )";
    
    if (!m_HUDShader->LoadFromSource(vertexSrc, fragmentSrc)) {
        std::cerr << "Failed to create HUD shader!" << std::endl;
        return false;
    }

    CreateQuadMesh();
    
    std::cout << "HUD Renderer initialized." << std::endl;
    return true;
}

void HUDRenderer::Shutdown() {
    if (m_QuadVAO) glDeleteVertexArrays(1, &m_QuadVAO);
    if (m_QuadVBO) glDeleteBuffers(1, &m_QuadVBO);
    if (m_QuadEBO) glDeleteBuffers(1, &m_QuadEBO);
    
    delete m_HUDShader;
    m_HUDShader = nullptr;
}

void HUDRenderer::BeginHUD() {
    // 2D rendering için depth test'i kapat
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void HUDRenderer::EndHUD() {
    glEnable(GL_DEPTH_TEST);
}

void HUDRenderer::DrawRect(float x, float y, float width, float height, const glm::vec4& color) {
    // Orthographic projection (screen space)
    glm::mat4 projection = glm::ortho(0.0f, m_ScreenWidth, 0.0f, m_ScreenHeight);
    
    // Model matrix (position ve scale)
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    
    m_HUDShader->Bind();
    m_HUDShader->SetMat4("uProjection", projection);
    m_HUDShader->SetMat4("uModel", model);
    m_HUDShader->SetVec4("uColor", color);
    m_HUDShader->SetInt("uUseTexture", 0);
    
    glBindVertexArray(m_QuadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void HUDRenderer::DrawTexture(Texture* texture, float x, float y, float width, float height) {
    if (!texture || !texture->IsLoaded()) return;

    glm::mat4 projection = glm::ortho(0.0f, m_ScreenWidth, 0.0f, m_ScreenHeight);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    
    m_HUDShader->Bind();
    m_HUDShader->SetMat4("uProjection", projection);
    m_HUDShader->SetMat4("uModel", model);
    m_HUDShader->SetVec4("uColor", glm::vec4(1.0f));
    m_HUDShader->SetInt("uUseTexture", 1);
    m_HUDShader->SetInt("uTexture", 0);
    
    texture->Bind(0);
    
    glBindVertexArray(m_QuadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void HUDRenderer::DrawCrosshair(float size, const glm::vec4& color) {
    float centerX = m_ScreenWidth * 0.5f;
    float centerY = m_ScreenHeight * 0.5f;
    float thickness = 2.0f;
    
    // Horizontal line
    DrawRect(centerX - size * 0.5f, centerY - thickness * 0.5f, size, thickness, color);
    
    // Vertical line
    DrawRect(centerX - thickness * 0.5f, centerY - size * 0.5f, thickness, size, color);
}

void HUDRenderer::DrawAmmoCounter(int current, int total, float x, float y) {
    // Basit ammo display (şimdilik text rendering yok, sadece bar)
    float barWidth = 200.0f;
    float barHeight = 20.0f;
    
    // Background
    DrawRect(x, y, barWidth, barHeight, glm::vec4(0.2f, 0.2f, 0.2f, 0.8f));
    
    // Current ammo bar
    float fillRatio = (float)current / (float)total;
    glm::vec4 ammoColor = fillRatio > 0.3f ? glm::vec4(0.3f, 0.8f, 0.3f, 0.9f) : glm::vec4(0.8f, 0.2f, 0.2f, 0.9f);
    DrawRect(x + 2, y + 2, (barWidth - 4) * fillRatio, barHeight - 4, ammoColor);
}

void HUDRenderer::DrawHealthBar(float health, float maxHealth, float x, float y, float width, float height) {
    // Background
    DrawRect(x, y, width, height, glm::vec4(0.2f, 0.2f, 0.2f, 0.8f));
    
    // Health bar
    float fillRatio = health / maxHealth;
    glm::vec4 healthColor;
    
    if (fillRatio > 0.6f) healthColor = glm::vec4(0.2f, 0.8f, 0.2f, 0.9f); // Yeşil
    else if (fillRatio > 0.3f) healthColor = glm::vec4(0.8f, 0.8f, 0.2f, 0.9f); // Sarı
    else healthColor = glm::vec4(0.8f, 0.2f, 0.2f, 0.9f); // Kırmızı
    
    DrawRect(x + 2, y + 2, (width - 4) * fillRatio, height - 4, healthColor);
}

void HUDRenderer::SetScreenSize(float width, float height) {
    m_ScreenWidth = width;
    m_ScreenHeight = height;
}

void HUDRenderer::CreateQuadMesh() {
    // Basit quad (screen-space coordinates 0-1)
    float vertices[] = {
        // Pos      // TexCoords
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        0, 3, 1
    };
    
    glGenVertexArrays(1, &m_QuadVAO);
    glGenBuffers(1, &m_QuadVBO);
    glGenBuffers(1, &m_QuadEBO);
    
    glBindVertexArray(m_QuadVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_QuadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    // TexCoords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
}

} // namespace Archura
