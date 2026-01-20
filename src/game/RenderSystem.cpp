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
        std::cerr << "Failed to load default shader!\n";
    }

    // Shadow Map Init
    InitShadowMap();

    m_DepthShader = std::make_unique<Shader>();
    if (!m_DepthShader->LoadFromFile("assets/shaders/depth.vert", "assets/shaders/depth.frag")) {
        std::cerr << "Failed to load depth shader!\n";
    }

    // Debug Mesh (Cube)
    m_DebugMesh = Mesh::CreateCube(1.0f);
    
    // std::cout << "RenderSystem initialized." << std::endl;
}

void RenderSystem::InitShadowMap() {
    glGenFramebuffers(1, &m_DepthMapFBO);
    
    glGenTextures(1, &m_DepthMapTexture);
    glBindTexture(GL_TEXTURE_2D, m_DepthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMapTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Shadow Framebuffer is not complete!\n";
        
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    

    // Lighting Setup
    // Isiklari topla
    struct LightData {
        glm::vec3 position;
        glm::vec3 direction; // For directional
        glm::vec3 color;
        float intensity;
        float range;
        int type; // 0 = Directional, 1 = Point
    };

    std::vector<LightData> lights;
    
    // Varsayilan isik (Gunes) eger hic isik yoksa
    bool hasLights = false;

    for (const auto& entityPtr : m_Scene->GetEntities()) {
        auto* lightComp = entityPtr->GetComponent<LightComponent>();
        auto* transform = entityPtr->GetComponent<Transform>();
        
        if (lightComp && transform) {
            LightData ld;
            ld.position = transform->position; // Point light pos
            
            // Rotation'dan direction cikarimi (Directional light icin)
            // Basitce Z ekseni rotasyonu varsayalim veya transform forward
            // Simdilik (0, -1, 0) varsayip rotation ile cevirmemiz lazim ama 
            // karmasiklastirmamak icin sadece position kullanalim.
            // Directional light icin position = direction origin gibi dusunulebilir simdilik.
            ld.direction = glm::vec3(-0.2f, -1.0f, -0.3f); // Sabit bir direction simdilik

            ld.color = lightComp->color;
            ld.intensity = lightComp->intensity;
            ld.range = lightComp->range;
            ld.type = (int)lightComp->type;
            
            lights.push_back(ld);
            hasLights = true;
            
            // Simdilik sadece ilk 4 isigi alalim (Shader siniri)
            if (lights.size() >= 4) break; 
        }
    }


    // Eger hic isik yoksa varsayilan bir isik ekle
    if (!hasLights) {
        LightData defaultLight;
        defaultLight.position = glm::vec3(5.0f, 10.0f, 5.0f);
        defaultLight.color = glm::vec3(1.0f);
        defaultLight.intensity = 1.0f;
        defaultLight.range = 100.0f;
        defaultLight.type = 1; // Point
        lights.push_back(defaultLight);
    }
    
    // --- 1. Pass: Render to Shadow Map (Directional Light only) ---
    // En yakin directional isigi bul (Gunes)
    // Simdilik list'teki type=0 olan ilk isigi alalim
    
    glm::vec3 lightPos = glm::vec3(0.0f);
    bool hasDirLight = false;
    
    for (const auto& l : lights) {
        if (l.type == 0) { // Directional
             // RenderSystem'de direction yerine position alanina "yon" degil de
             // isigin geldigi konumu yaziyoruz (Simule etmek icin).
             // Directional light sonsuz uzakta ama golge matrisi icin bir pozisyona ihtiyacimiz var.
             // Editor'de light objesini yukari tasiyarak bu pozisyonu belirleyebiliriz.
             lightPos = l.position;
             hasDirLight = true;
             break;
        }
    }
    
    // Eger directional light yoksa, golge pass'ini deaktif edebiliriz ama
    // shader texture bekledigi icin bos texture bind etmemiz gerekebilir.
    // Simdilik directional light varsa render edelim.
    
    if (hasDirLight && m_DepthShader) {
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        
        // Light Space Matrix
        float near_plane = 1.0f, far_plane = 100.0f; // Alan genisligine gore ayarlanmali
        // Orthographic projection for directional light
        glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, near_plane, far_plane);
        
        // Light View: Isik pozisyonundan (0,0,0)'a (veya sahne merkezine) bakis
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        
        m_LightSpaceMatrix = lightProjection * lightView;
        
        m_DepthShader->Bind();
        m_DepthShader->SetMat4("lightSpaceMatrix", m_LightSpaceMatrix);
        
        // Tum sahneyi depth icin render et
        // Batch mantigini burada da kullanabiliriz ama basitlik icin direkt loop
        // (Sadece MeshRenderer olanlari)
        for (const auto& batch : batches) {
             // Texture/Shader onemsiz, sadece geometry (model matrix)
             batch.mesh->DrawInstanced(m_DepthShader.get(), batch.instanceMatrices);
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        // No directional light, reset matrix to identity or keep zero
        // Maybe clear texture to white (depth 1.0) so everything is lit
        glBindFramebuffer(GL_FRAMEBUFFER, m_DepthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // --- 2. Pass: Normal Rendering ---
    
    // Reset Viewport
    glViewport(0, 0, window->GetWidth(), window->GetHeight());
    
    for (const auto& batch : batches) {
        if (batch.instanceMatrices.empty()) continue;
        
        Shader* shader = batch.shader;
        shader->Bind();
        
        shader->SetMat4("uView", view);
        shader->SetMat4("uProjection", projection);
        shader->SetVec3("uViewPos", m_Camera->GetPosition());

        // Shadow Map Uniforms
        shader->SetMat4("uLightSpaceMatrix", m_LightSpaceMatrix);
        shader->SetInt("uShadowMap", 1); // Texture Unit 1
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_DepthMapTexture);

        // Pass Lights to Shader
        // Note: Shader uniform array desteklemeli: uniform Light uLights[4]; ve int uLightCount;
        shader->SetInt("uLightCount", (int)lights.size());
        
        for (size_t i = 0; i < lights.size(); i++) {
            std::string base = "uLights[" + std::to_string(i) + "]";
            shader->SetVec3(base + ".position", lights[i].position);
            // shader->SetVec3(base + ".direction", lights[i].direction); // Future
            shader->SetVec3(base + ".color", lights[i].color);
            shader->SetFloat(base + ".intensity", lights[i].intensity);
            shader->SetFloat(base + ".range", lights[i].range);
            shader->SetInt(base + ".type", lights[i].type);
        }
        
        // Material
        if (batch.texture) {
            batch.texture->Bind(0); // Unit 0
            shader->SetInt("uTexture", 0);
            shader->SetInt("uUseTexture", 1);
            shader->SetVec3("uDiffuse", glm::vec3(1.0f));
        } else {
            shader->SetInt("uUseTexture", 0);
            shader->SetVec3("uDiffuse", batch.color); 
        }
        
        // Tek seferde ciz (Instanced)
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
    // Set minimal lighting for wireframe debug rendering
    m_DefaultShader->SetVec3("uAmbient", glm::vec3(1.0f));
    m_DefaultShader->SetVec3("uDiffuse", glm::vec3(1.0f));
    m_DefaultShader->SetVec3("uSpecular", glm::vec3(0.0f));
    m_DefaultShader->SetFloat("uShininess", 1.0f);
    m_DefaultShader->SetInt("uLightCount", 0); // No lights for debug wireframe
 

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
