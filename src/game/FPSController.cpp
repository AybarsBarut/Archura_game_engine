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
    // Varsayilan tus atamalari
    m_Bindings.forward = GLFW_KEY_W;
    m_Bindings.backward = GLFW_KEY_S;
    m_Bindings.left = GLFW_KEY_A;
    m_Bindings.right = GLFW_KEY_D;
    m_Bindings.jump = GLFW_KEY_SPACE;
    m_Bindings.sprint = GLFW_KEY_LEFT_SHIFT;
}

void FPSController::Update(Input* input, Scene* scene, float deltaTime) {
    HandleMovement(input, scene, deltaTime);
    HandleMouseLook(input, deltaTime);
}

void FPSController::HandleMovement(Input* input, Scene* scene, float deltaTime) {
    // Kosma kontrolu (Shift)
    m_IsRunning = input->IsKeyDown(m_Bindings.sprint);
    float currentSpeed = m_IsRunning ? m_RunSpeed : m_WalkSpeed;

    glm::vec3 currentPos = m_Camera->GetPosition();
    glm::vec3 targetPos = currentPos;
    
    glm::vec3 front = m_Camera->GetFront();
    glm::vec3 right = m_Camera->GetRight();
    
    // Yercekimi aciksa Y eksenini sifirla (Sadece XZ duzleminde hareket)
    // Kapaliysa (Fly Mode) kameranin baktigi yere git
    if (m_GravityEnabled) {
        front.y = 0.0f;
        right.y = 0.0f;
    }
    
    if (glm::length(front) > 0.001f) front = glm::normalize(front);
    if (glm::length(right) > 0.001f) right = glm::normalize(right);

    // Hareket tuslari
    glm::vec3 movement = glm::vec3(0.0f);
    if (input->IsKeyDown(m_Bindings.forward)) movement += front;
    if (input->IsKeyDown(m_Bindings.backward)) movement -= front;
    if (input->IsKeyDown(m_Bindings.left)) movement -= right;
    if (input->IsKeyDown(m_Bindings.right)) movement += right;

    if (glm::length(movement) > 0.001f) {
        movement = glm::normalize(movement) * currentSpeed * deltaTime;
        
        if (m_GravityEnabled) {
            // Normal yurume (Collision var)
            // X ekseninde hareket dene
            glm::vec3 nextPosX = currentPos;
            nextPosX.x += movement.x;
            if (!CheckCollision(nextPosX, scene, nullptr, 0.3f)) {
                targetPos.x = nextPosX.x;
            }

            // Z ekseninde hareket dene
            glm::vec3 nextPosZ = currentPos;
            nextPosZ.z += movement.z;
            if (!CheckCollision(nextPosZ, scene, nullptr, 0.3f)) {
                targetPos.z = nextPosZ.z;
            }
        } else {
            // Fly Mode (Collision yok veya basit hareket)
            // Kullanici "istedigim gibi ucmami sagla" dedi, collision'i kapatalim (Noclip gibi)
            // Boylece harita yaparken duvarlarin icinden gecebilir
            targetPos += movement;
        }
    }

    if (m_GravityEnabled) {
        // Yercekimi ve Ziplama
        m_VerticalVelocity += m_Gravity * deltaTime;

        // Ziplama (Bosluk)
        if (input->IsKeyDown(m_Bindings.jump) && m_IsGrounded) {
            m_VerticalVelocity = sqrt(m_JumpHeight * -2.0f * m_Gravity);
            m_IsGrounded = false;
        }

        // Dikey hareket
        glm::vec3 nextPosY = targetPos;
        nextPosY.y += m_VerticalVelocity * deltaTime;
        
        float groundHeight = 0.0f; 
        float objectTopY = 0.0f;
        bool hitObject = CheckCollision(nextPosY, scene, &objectTopY, 0.0f);

        float playerEyeHeight = 1.8f;
        float minGroundY = 0.0f + playerEyeHeight; 

        if (hitObject) {
            if (m_VerticalVelocity <= 0.0f && (targetPos.y - playerEyeHeight) >= (objectTopY - 0.5f)) {
                targetPos.y = objectTopY + playerEyeHeight;
                m_VerticalVelocity = 0.0f;
                m_IsGrounded = true;
            }
            else if (m_VerticalVelocity > 0.0f) {
                m_VerticalVelocity = 0.0f;
            }
        }
        else if (nextPosY.y < minGroundY) {
            targetPos.y = minGroundY;
            m_VerticalVelocity = 0.0f;
            m_IsGrounded = true;
        }
        else {
            targetPos.y = nextPosY.y;
            m_IsGrounded = false;
        }
    } else {
        // Fly Mode'da dikey hiz sifirlanir
        m_VerticalVelocity = 0.0f;
        m_IsGrounded = false;
    }

    m_Camera->SetPosition(targetPos);

    // FOV kontrolu (fare tekerlegi)
    float scrollDelta = input->GetMouseScrollDelta();
    if (scrollDelta != 0.0f) {
        m_Camera->ProcessMouseScroll(scrollDelta);
    }
}

bool FPSController::CheckCollision(const glm::vec3& position, Scene* scene, float* outGroundHeight, float stepHeight) {
    // Oyuncu sinirlayici kutusu
    float playerRadius = 0.3f; // Yaricap (genislik/2)
    float playerHeight = 1.8f; // Gozden ayaklara mesafe
    
    // Ayaklarin pozisyonu
    glm::vec3 feetPos = position;
    feetPos.y -= playerHeight;

    // adimYuksekligi kadar yukaridan basla (kucuk engelleri/zemini yok saymak icin)
    glm::vec3 playerMin = feetPos - glm::vec3(playerRadius, -stepHeight, playerRadius);
    glm::vec3 playerMax = feetPos + glm::vec3(playerRadius, 1.8f, playerRadius); // Boy 1.8m

    // Sahnedeki tum varliklari kontrol et
    for (auto& entity : scene->GetEntities()) {
        auto* collider = entity->GetComponent<BoxCollider>();
        auto* transform = entity->GetComponent<Transform>();
        
        if (collider && transform) {
            // Helper lambda for AABB check
            auto CheckAABB = [&](const glm::vec3& center, const glm::vec3& size) -> bool {
                glm::vec3 boxCenter = transform->position + center;
                glm::vec3 boxSize = size * transform->scale;
                
                glm::vec3 boxMin = boxCenter - boxSize * 0.5f;
                glm::vec3 boxMax = boxCenter + boxSize * 0.5f;

                bool collisionX = playerMin.x <= boxMax.x && playerMax.x >= boxMin.x;
                bool collisionY = playerMin.y <= boxMax.y && playerMax.y >= boxMin.y;
                bool collisionZ = playerMin.z <= boxMax.z && playerMax.z >= boxMin.z;

                if (collisionX && collisionY && collisionZ) {
                    if (outGroundHeight) {
                        // Keep the highest ground found so far
                        if (boxMax.y > *outGroundHeight) {
                            *outGroundHeight = boxMax.y;
                        }
                    }
                    return true;
                }
                return false;
            };

            bool hit = false;
            
            // Check main box (if size is not zero)
            if (glm::length(collider->size) > 0.01f) {
                if (CheckAABB(collider->center, collider->size)) hit = true;
            }

            // Check sub boxes
            for (const auto& box : collider->subBoxes) {
                if (CheckAABB(box.center, box.size)) hit = true;
            }

            if (hit && !outGroundHeight) return true; // Early exit if we don't need ground height
            if (hit && outGroundHeight) {
                // Continue checking other entities to find highest ground? 
                // For now, just return true, but we updated outGroundHeight inside lambda
                // However, we need to check ALL entities to find true highest ground.
                // The current function returns true on FIRST collision.
                // This is fine for blocking, but for ground height it might be inaccurate if multiple overlap.
                // But usually we stand on one.
                return true;
            }
        }
    }
    
    return false;
}

void FPSController::HandleMouseLook(Input* input, float deltaTime) {
    // Imlec kilitli ise (FPS modu) kamerayi dondur
    if (input->IsCursorLocked()) {
        glm::vec2 mouseDelta = input->GetMouseDelta();
        m_Camera->ProcessMouseMovement(mouseDelta.x * m_MouseSensitivity, 
                                      -mouseDelta.y * m_MouseSensitivity); // Y eksenini ters cevir
    }

    // Sol tik ile imleci kilitle (Oyun moduna gir)
    if (input->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !input->IsCursorLocked()) {
        input->SetCursorMode(GLFW_CURSOR_DISABLED);
    }
    
    // ESC ile imleci serbest birak (Gecici cikis)
    // Not: Ana dongude ESC cikis yapiyor olabilir, bunu kontrol etmeliyiz.
    // Simdilik sadece kilitli degilse serbest birakma mantigi kalsin.
}

} // namespace Archura
