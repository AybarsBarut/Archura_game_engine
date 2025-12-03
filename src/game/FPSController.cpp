#include "FPSController.h"
#include "../input/Input.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace Archura {

FPSController::FPSController(Camera* camera)
    : m_Camera(camera)
{
}

void FPSController::Update(Input* input, Scene* scene, float deltaTime) {
    HandleMovement(input, scene, deltaTime);
    HandleMouseLook(input, deltaTime);
}

void FPSController::HandleMovement(Input* input, Scene* scene, float deltaTime) {
    // Koşma kontrolü (Shift)
    m_IsRunning = input->IsKeyDown(GLFW_KEY_LEFT_SHIFT);
    float currentSpeed = m_IsRunning ? m_RunSpeed : m_WalkSpeed;

    glm::vec3 currentPos = m_Camera->GetPosition();
    glm::vec3 targetPos = currentPos;
    
    glm::vec3 front = m_Camera->GetFront();
    glm::vec3 right = m_Camera->GetRight();
    
    // Y eksenini sifirla (Sadece XZ duzleminde hareket)
    front.y = 0.0f;
    right.y = 0.0f;
    
    if (glm::length(front) > 0.001f) front = glm::normalize(front);
    if (glm::length(right) > 0.001f) right = glm::normalize(right);

    // WASD hareketi - Her ekseni ayri ayri kontrol et (kayma hareketi icin)
    glm::vec3 movement = glm::vec3(0.0f);
    if (input->IsKeyDown(GLFW_KEY_W)) movement += front;
    if (input->IsKeyDown(GLFW_KEY_S)) movement -= front;
    if (input->IsKeyDown(GLFW_KEY_A)) movement -= right;
    if (input->IsKeyDown(GLFW_KEY_D)) movement += right;

    if (glm::length(movement) > 0.001f) {
        movement = glm::normalize(movement) * currentSpeed * deltaTime;
        
        // X ekseninde hareket dene
        glm::vec3 nextPosX = currentPos;
        nextPosX.x += movement.x;
        // Yatay harekette stepHeight kullanarak zemine takilmayi onle (0.2f)
        if (!CheckCollision(nextPosX, scene, nullptr, 0.2f)) {
            targetPos.x = nextPosX.x;
        }

        // Z ekseninde hareket dene
        glm::vec3 nextPosZ = currentPos;
        nextPosZ.z += movement.z;
        if (!CheckCollision(nextPosZ, scene, nullptr, 0.2f)) {
            targetPos.z = nextPosZ.z;
        }
    }

    // Yercekimi ve Ziplama
    m_VerticalVelocity += m_Gravity * deltaTime;

    // Ziplama (Space)
    if (input->IsKeyDown(GLFW_KEY_SPACE) && m_IsGrounded) {
        m_VerticalVelocity = sqrt(m_JumpHeight * -2.0f * m_Gravity);
        m_IsGrounded = false;
    }

    // Dikey hareket
    glm::vec3 nextPosY = targetPos;
    nextPosY.y += m_VerticalVelocity * deltaTime;
    
    float groundHeight = 0.0f; // Varsayilan zemin (Mesh plane y=0)
    float objectTopY = 0.0f;
    // Dikey harekette stepHeight 0 olmali ki tam inis yapabilelim
    bool hitObject = CheckCollision(nextPosY, scene, &objectTopY, 0.0f);

    // Zemin kontrolu (Basit duzlem carpismasi)
    // Oyuncu boyu yaklasik 1.8 birim (Goz hizasi)
    // Ayaklar = position.y - 1.8f
    float playerEyeHeight = 1.8f;
    float minGroundY = 0.0f + playerEyeHeight; // Zemin (Y=0) uzerinde durus pozisyonu

    if (hitObject) {
        // Eger dusuyorsak ve ayagimiz objenin ustundeyse, inis yap
        // Ayak pozisyonu: nextPosY.y - 1.8f
        // Obje ustu: objectTopY
        
        // Toleransli kontrol: Ayaklar objenin biraz icine girmis olabilir
        if (m_VerticalVelocity <= 0.0f && (targetPos.y - playerEyeHeight) >= (objectTopY - 0.5f)) {
            targetPos.y = objectTopY + playerEyeHeight;
            m_VerticalVelocity = 0.0f;
            m_IsGrounded = true;
        }
        else if (m_VerticalVelocity > 0.0f) {
            // Kafayi vurduk
            m_VerticalVelocity = 0.0f;
            // targetPos.y degismez, yukari cikamaz
        }
    }
    else if (nextPosY.y < minGroundY) {
        // Zemine carptik
        targetPos.y = minGroundY;
        m_VerticalVelocity = 0.0f;
        m_IsGrounded = true;
    }
    else {
        // Havada serbest hareket
        targetPos.y = nextPosY.y;
        m_IsGrounded = false;
    }

    m_Camera->SetPosition(targetPos);

    // FOV kontrolü (mouse scroll)
    float scrollDelta = input->GetMouseScrollDelta();
    if (scrollDelta != 0.0f) {
        m_Camera->ProcessMouseScroll(scrollDelta);
    }
}

bool FPSController::CheckCollision(const glm::vec3& position, Scene* scene, float* outGroundHeight, float stepHeight) {
    // Oyuncu bounding box
    float playerRadius = 0.3f; // Yaricap (genislik/2)
    float playerHeight = 1.8f; // Gozden ayaklara mesafe
    
    // Ayaklarin pozisyonu
    glm::vec3 feetPos = position;
    feetPos.y -= playerHeight;

    // stepHeight kadar yukaridan basla (kucuk engelleri/zemini yok saymak icin)
    glm::vec3 playerMin = feetPos - glm::vec3(playerRadius, -stepHeight, playerRadius);
    glm::vec3 playerMax = feetPos + glm::vec3(playerRadius, 1.8f, playerRadius); // Boy 1.8m

    // Sahnedeki tum entity'leri kontrol et
    for (auto& entity : scene->GetEntities()) {
        auto* collider = entity->GetComponent<BoxCollider>();
        auto* transform = entity->GetComponent<Transform>();
        
        if (collider && transform) {
            // AABB carpisma testi
            glm::vec3 boxCenter = transform->position + collider->center;
            glm::vec3 boxSize = collider->size * transform->scale;
            
            glm::vec3 boxMin = boxCenter - boxSize * 0.5f;
            glm::vec3 boxMax = boxCenter + boxSize * 0.5f;

            // AABB vs AABB intersection
            bool collisionX = playerMin.x <= boxMax.x && playerMax.x >= boxMin.x;
            bool collisionY = playerMin.y <= boxMax.y && playerMax.y >= boxMin.y;
            bool collisionZ = playerMin.z <= boxMax.z && playerMax.z >= boxMin.z;

            if (collisionX && collisionY && collisionZ) {
                if (outGroundHeight) {
                    *outGroundHeight = boxMax.y;
                }
                return true;
            }
        }
    }
    
    return false;
}

void FPSController::HandleMouseLook(Input* input, float deltaTime) {
    // Cursor kilitli ise (FPS modu) kamerayi dondur
    if (input->IsCursorLocked()) {
        glm::vec2 mouseDelta = input->GetMouseDelta();
        m_Camera->ProcessMouseMovement(mouseDelta.x * m_MouseSensitivity, 
                                      -mouseDelta.y * m_MouseSensitivity); // Y eksenini ters çevir
    }

    // Sol tik ile cursoru kilitle (Oyun moduna gir)
    if (input->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !input->IsCursorLocked()) {
        input->SetCursorMode(GLFW_CURSOR_DISABLED);
    }
    
    // ESC ile cursoru serbest birak (Gecici cikis)
    // Not: Main loop'ta ESC cikis yapiyor olabilir, bunu kontrol etmeliyiz.
    // Simdilik sadece kilitli degilse serbest birakma mantigi kalsin.
}

} // namespace Archura
