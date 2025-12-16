#include "RenderSystem.h"
#include "../core/Engine.h"
#include "../core/Window.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include "../rendering/Mesh.h"
#include "../rendering/Texture.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
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

    // Debug Mesh (Cube)
    m_DebugMesh = Mesh::CreateCube(1.0f);
    
    // std::cout << "RenderSystem initialized." << std::endl;
}

void RenderSystem::Update(float deltaTime) {
    if (!m_Scene || !m_Camera) return;

    // Reset State to defaults for normal rendering
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // Kamera matrisleri
    glm::mat4 view = m_Camera->GetViewMatrix();
    Window* window = Engine::Get().GetWindow();
    glm::mat4 projection = m_Camera->GetProjectionMatrix(window->GetAspectRatio());

    // Batch Rendering Logic
    // Gruplandirma: Mesh -> (Texture/Shader) -> Transforms
    // Simdilik Shader ve Texture tek tip varsayalim veya basitlestirelim.
    // Map: Mesh* -> List of ModelMatrices
    // Isin dogrusu: Material yapisi olmali. Simdilik MeshRenderer icindeki mesh ve texture'a gore gruplayalim.
    
    struct RenderBatch {
        Mesh* mesh;
        Shader* shader;
        Texture* texture;
        glm::vec3 color; // Color override
        std::vector<glm::mat4> instanceMatrices;
    };
    
    // Basit bir vector kullanalim, her unique (mesh, texture, shader) kombinasyonu icin bir batch
    std::vector<RenderBatch> batches;
    
    // Cull edilenler
    int culledCount = 0;
    int renderedCount = 0;
    
    // 1. Collect
    for (const auto& entityPtr : m_Scene->GetEntities()) {
        auto* meshRenderer = entityPtr->GetComponent<MeshRenderer>();
        auto* transform = entityPtr->GetComponent<Transform>();
        
        if (!meshRenderer || !transform || !meshRenderer->mesh) continue;

        // Frustum Culling
        glm::vec3 entityPos = transform->position;
            glm::vec3 camPos = m_Camera->GetPosition();
            float distance = glm::length(entityPos - camPos);
            if (distance > 1000.0f) { // Uzakligi arttirdim
                culledCount++;
                continue;
        }

        // Batch bul veya olustur
        Shader* targetShader = meshRenderer->shader ? meshRenderer->shader : m_DefaultShader.get();
        Texture* targetTexture = meshRenderer->texture;
        
        bool found = false;
        for (auto& batch : batches) {
            if (batch.mesh == meshRenderer->mesh && 
                batch.shader == targetShader && 
                batch.texture == targetTexture &&
                batch.color == meshRenderer->color) { // Renk de ayni olmali
                
                batch.instanceMatrices.push_back(transform->GetModelMatrix());
                found = true;
                break;
            }
        }
        
        if (!found) {
            RenderBatch newBatch;
            newBatch.mesh = meshRenderer->mesh;
            newBatch.shader = targetShader;
            newBatch.texture = targetTexture;
            newBatch.color = meshRenderer->color;
            newBatch.instanceMatrices.push_back(transform->GetModelMatrix());
            batches.push_back(newBatch);
        }
    }
    
    // 2. Render
    for (const auto& batch : batches) {
        if (batch.instanceMatrices.empty()) continue;
        
        Shader* shader = batch.shader;
        shader->Bind();
        
        shader->SetMat4("uView", view);
        shader->SetMat4("uProjection", projection);
        shader->SetVec3("uLightPos", m_LightPos);
        shader->SetVec3("uLightColor", m_LightColor);
        shader->SetVec3("uViewPos", m_Camera->GetPosition());
        
        // Material
        if (batch.texture) {
            batch.texture->Bind(0);
            shader->SetInt("uTexture", 0);
            shader->SetInt("uUseTexture", 1);
            shader->SetVec3("uDiffuse", glm::vec3(1.0f));
        } else {
            shader->SetInt("uUseTexture", 0);
            shader->SetVec3("uDiffuse", batch.color); 
        }
        
        // Tek seferde ciz (Instanced) ya da tek tek (Fallback)
        // Mesh::DrawInstanced metodumuz var artik
        batch.mesh->DrawInstanced(shader, batch.instanceMatrices);
        
        renderedCount += batch.instanceMatrices.size();
    }

    // Hata ayiklama: her 60 karede bir culling istatistiklerini yazdir
    static int frameCount = 0;
    if (++frameCount >= 60) {
        // std::cout << "Rendered: " << renderedCount << " | Culled: " << culledCount << std::endl;
        frameCount = 0;
    }
}



void RenderSystem::DrawColliders() {
    if (!m_Scene || !m_Camera || !m_DebugMesh || !m_DefaultShader) return;
    if (m_DefaultShader->GetProgramID() == 0) return;

    // Save previous state (optional but good practice)
    GLint polygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, polygonMode);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE); 
    glDisable(GL_DEPTH_TEST);

    m_DefaultShader->Bind();
    
    // Camera uniforms
    glm::mat4 view = m_Camera->GetViewMatrix();
    glm::mat4 projection = m_Camera->GetProjectionMatrix(Engine::Get().GetWindow()->GetAspectRatio());

    m_DefaultShader->SetMat4("uView", view);
    m_DefaultShader->SetMat4("uProjection", projection);
    m_DefaultShader->SetInt("uUseTexture", 0);
    // Light is minimal for wireframe
    m_DefaultShader->SetVec3("uLightColor", glm::vec3(1.0f)); 

    for (const auto& entityPtr : m_Scene->GetEntities()) {
        auto* collider = entityPtr->GetComponent<BoxCollider>();
        auto* transform = entityPtr->GetComponent<Transform>();
        
        if (!collider || !transform) continue;

        // Color based on isTrigger
        glm::vec3 color = collider->isTrigger ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(0.0f, 1.0f, 0.0f);
        m_DefaultShader->SetVec3("uDiffuse", color);

        // Calculate Transform: EntityModel * ColliderOffset * ColliderSize
        glm::mat4 model = transform->GetModelMatrix();
        model = glm::translate(model, collider->center);
        model = glm::scale(model, collider->size);
        
        m_DefaultShader->SetMat4("uModel", model);
        
        m_DebugMesh->Draw(m_DefaultShader.get());
    }

    // Restore state
    glPolygonMode(GL_FRONT, polygonMode[0]);
    glPolygonMode(GL_BACK, polygonMode[1]);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void RenderSystem::Shutdown() {
    m_DefaultShader.reset();
    if (m_DebugMesh) {
        delete m_DebugMesh;
        m_DebugMesh = nullptr;
    }
    // std::cout << "RenderSystem shut down." << std::endl;
}

} // namespace Archura
