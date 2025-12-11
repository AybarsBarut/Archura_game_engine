#include "Renderer.h"
#include <glad/glad.h>
#include <iostream>

namespace Archura {

bool Renderer::Init() {
    // std::cout << "Renderer initializing..." << std::endl;
    
    // OpenGL durum ayarlari
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Yuz kirpma - performans icin
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // MSAA - GTX 1050'de sorunsuz calisir
    glEnable(GL_MULTISAMPLE);
    
    SetClearColor(m_ClearColor);
    
    // std::cout << "Renderer initialized successfully!" << std::endl;
    return true;
}

void Renderer::Shutdown() {
    // std::cout << "Renderer shutting down..." << std::endl;
}

void Renderer::BeginFrame() {
    m_Stats.Reset();
    Clear();
}

void Renderer::EndFrame() {
    // Kare sonu islemleri
    // - UI rendering (ImGui)
    // - Son isleme efektleri
    // vs...
}

void Renderer::SetClearColor(const glm::vec4& color) {
    m_ClearColor = color;
    glClearColor(color.r, color.g, color.b, color.a);
}

void Renderer::Clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

} // namespace Archura
