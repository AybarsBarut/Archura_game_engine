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
    // Oyuncu sinirlayici kutusu (Silindir gibi dusunelim ama AABB kontrolu yapiyoruz)
    float playerRadius = 0.3f; 
    float playerHeight = 1.8f; 
    
    // Ayaklarin pozisyonu
    glm::vec3 feetPos = position;
    feetPos.y -= playerHeight;

    // Sahnedeki tum varliklari kontrol et
    for (auto& entity : scene->GetEntities()) {
        auto* collider = entity->GetComponent<BoxCollider>();
        auto* transform = entity->GetComponent<Transform>();
        
        if (collider && transform) {
            // Model Matrisini Olustur (T * R * S)
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform->position);
            model = glm::rotate(model, glm::radians(transform->rotation.x), glm::vec3(1, 0, 0));
            model = glm::rotate(model, glm::radians(transform->rotation.y), glm::vec3(0, 1, 0));
            model = glm::rotate(model, glm::radians(transform->rotation.z), glm::vec3(0, 0, 1));
            
            glm::mat4 invModel = glm::inverse(model);

            // Helper lambda for OBB check
            auto CheckOBB = [&](const glm::vec3& localCenter, const glm::vec3& localSize) -> bool {
                // Kutu Boyutu (Scale dahil)
                glm::vec3 boxHalfSize = localSize * transform->scale * 0.5f;
                
                // Oyuncunun ayak pozisyonunu yerel uzaya cevir
                // Oyuncuyu bir nokta/küre gibi dusunup, kutuyu genisletecegiz
                glm::vec4 localFeet4 = invModel * glm::vec4(feetPos, 1.0f);
                glm::vec3 localFeet = glm::vec3(localFeet4);
                
                // Genisletilmis Kutu Sinirlari (Local Space)
                glm::vec3 expandedMin = localCenter - boxHalfSize;
                glm::vec3 expandedMax = localCenter + boxHalfSize;
                
                // Oyuncu Yaricapi (X ve Z)
                expandedMin.x -= playerRadius; expandedMax.x += playerRadius;
                expandedMin.z -= playerRadius; expandedMax.z += playerRadius;
                
                // Test Noktasi: Ayaklarin biraz ustu (stepHeight kadar)
                glm::vec4 testPos4 = invModel * glm::vec4(feetPos + glm::vec3(0, stepHeight + 0.1f, 0), 1.0f);
                glm::vec3 testPos = glm::vec3(testPos4);
                
                bool overlapX = testPos.x >= expandedMin.x && testPos.x <= expandedMax.x;
                bool overlapY = testPos.y >= expandedMin.y && testPos.y <= expandedMax.y; // Y carpismasi (boy)
                bool overlapZ = testPos.z >= expandedMin.z && testPos.z <= expandedMax.z;
                
                if (overlapX && overlapZ) {
                    // X ve Z olarak icindeyiz. Y olarak durum ne?
                    
                    float boxTopLocal = localCenter.y + boxHalfSize.y;
                    
                    // Eger test noktasi (ayak+step) kutunun ust sinirindan asagidaysa ve alt sinirindan yukaridaysa -> Carpisma
                    if (testPos.y < boxTopLocal && testPos.y > (localCenter.y - boxHalfSize.y)) {
                        return true; // Icine girmisiz
                    }
                    
                    // Zemin tespiti (outGroundHeight)
                    if (outGroundHeight) {
                        // Oyuncu kutunun ustunde (veya cok yakininda)
                        // Local'den World'e cevir (Sadece Y lazim degil, nokta lazim)
                        // Yaklasik Zemin: Oyuncunun bulundugu XZ konumundaki Y yuksekligi.
                        // Simdilik: Eger XZ icindeysek ve Y olarak ustundeysek, zemin olarak kabul et.
                        if (testPos.y >= boxTopLocal) {
                            glm::vec3 topPointLocal = testPos; 
                            topPointLocal.y = boxTopLocal;
                            glm::vec4 topPointWorld = model * glm::vec4(topPointLocal, 1.0f);
                            
                            if (topPointWorld.y > *outGroundHeight) {
                                *outGroundHeight = topPointWorld.y;
                            }
                        }
                    }
                }
                return false;
            };

            bool hit = false;
            
            // Check main box
            if (glm::length(collider->size) > 0.01f) {
                if (CheckOBB(collider->center, collider->size)) hit = true;
            }

            // Check sub boxes
            for (const auto& box : collider->subBoxes) {
                if (CheckOBB(box.center, box.size)) hit = true;
            }

            if (hit && !outGroundHeight) return true; 
            if (hit && outGroundHeight) return true;
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
