#include "RenderSystem.h"
#include "../core/Engine.h"
#include "../core/Window.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include "../rendering/Mesh.h"
#include "../rendering/Texture.h"
#include <iostream>

namespace Archura {

RenderSystem::RenderSystem(Camera* camera)
    : m_Camera(camera)
{
}

RenderSystem::~RenderSystem() = default;

void RenderSystem::Init(Scene* scene) {
    System::Init(scene);
    
    // Varsayilan shader'i yukle
    m_DefaultShader = std::make_unique<Shader>();
    if (!m_DefaultShader->LoadFromFile("assets/shaders/basic.vert", "assets/shaders/basic.frag")) {
        std::cerr << "Failed to load default shader!" << std::endl;
    }
    
    std::cout << "RenderSystem initialized." << std::endl;
}

void RenderSystem::Update(float deltaTime) {
    if (!m_Scene || !m_Camera) return;

    // Kamera matrisleri
    glm::mat4 view = m_Camera->GetViewMatrix();
    Window* window = Engine::Get().GetWindow();
    glm::mat4 projection = m_Camera->GetProjectionMatrix(window->GetAspectRatio());

    int renderedCount = 0;
    int culledCount = 0;

    // Tum varliklari isle
    for (const auto& entityPtr : m_Scene->GetEntities()) {
        auto* meshRenderer = entityPtr->GetComponent<MeshRenderer>();
        auto* transform = entityPtr->GetComponent<Transform>();
        
        if (meshRenderer && transform && meshRenderer->mesh) {
            // Basit frustum culling - kamera gorus alani disindaysa isleme
            glm::vec3 entityPos = transform->position;
            glm::vec3 camPos = m_Camera->GetPosition();
            glm::vec3 toEntity = entityPos - camPos;
            float distance = glm::length(toEntity);
            
            // Cok uzaktaysa (100 birimden fazla) isleme
            if (distance > 100.0f) {
                culledCount++;
                continue;
            }

            renderedCount++;
            
            Shader* shader = meshRenderer->shader ? meshRenderer->shader : m_DefaultShader.get();
            
            if (shader) {
                shader->Bind();
                
                // Matrisler
                shader->SetMat4("uModel", transform->GetModelMatrix());
                shader->SetMat4("uView", view);
                shader->SetMat4("uProjection", projection);
                
                // Aydinlatma
                shader->SetVec3("uLightPos", m_LightPos);
                shader->SetVec3("uLightColor", m_LightColor);
                shader->SetVec3("uViewPos", m_Camera->GetPosition());
                
                // Materyal (basit, sabit degerler)
                shader->SetVec3("uAmbient", glm::vec3(0.2f));
                shader->SetVec3("uDiffuse", meshRenderer->color);
                shader->SetVec3("uSpecular", glm::vec3(0.5f));
                shader->SetVec3("uSpecular", glm::vec3(0.5f));
                shader->SetFloat("uShininess", 32.0f);
                
                // Doku
                if (meshRenderer->texture) {
                    meshRenderer->texture->Bind(0);
                    shader->SetInt("uTexture", 0);
                    shader->SetInt("uUseTexture", 1);
                } else {
                    shader->SetInt("uUseTexture", 0);
                }
                
                // Modeli ciz
                meshRenderer->mesh->Draw(shader);
            }
        }
    }

    // Hata ayiklama: her 60 karede bir culling istatistiklerini yazdir
    static int frameCount = 0;
    if (++frameCount >= 60) {
        // std::cout << "Rendered: " << renderedCount << " | Culled: " << culledCount << std::endl;
        frameCount = 0;
    }
}

void RenderSystem::Shutdown() {
    m_DefaultShader.reset();
    std::cout << "RenderSystem shut down." << std::endl;
}

} // namespace Archura
