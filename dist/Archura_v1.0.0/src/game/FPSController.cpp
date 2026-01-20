#include "FPSController.h"
#include "../input/Input.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>

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
    // Mantling (Tirmanma) Durumu
    if (m_IsMantling) {
        m_MantleTimer += deltaTime;
        float t = glm::clamp(m_MantleTimer / m_MantleDuration, 0.0f, 1.0f);
        
        // Smooth step interpolation (daha yumusak gecis)
        t = t * t * (3.0f - 2.0f * t);

        if (m_MantleTimer >= m_MantleDuration) {
            m_Camera->SetPosition(m_MantleTargetPos);
            m_IsMantling = false;
            m_VerticalVelocity = 0.0f;
            m_IsGrounded = true;
        } else {
            glm::vec3 newPos = glm::mix(m_MantleStartPos, m_MantleTargetPos, t);
            m_Camera->SetPosition(newPos);
        }
        return; // Normal hareketi atla
    }

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
            // Sıkışma kontrolü (Eğer karakter bir objenin içindeyse harekete izin ver)
            // Radius 0.1f yapildi (daha az hassas, sadece gercekten icindeyse)
            bool isStuck = CheckCollision(currentPos, scene, nullptr, 0.1f);

            // X ekseninde hareket dene
            glm::vec3 nextPosX = currentPos;
            nextPosX.x += movement.x;
            if (isStuck || !CheckCollision(nextPosX, scene, nullptr, 0.3f)) {
                targetPos.x = nextPosX.x;
            }

            // Z ekseninde hareket dene
            glm::vec3 nextPosZ = currentPos;
            nextPosZ.z += movement.z;
            if (isStuck || !CheckCollision(nextPosZ, scene, nullptr, 0.3f)) {
                targetPos.z = nextPosZ.z;
            }
        } else {
            // Fly Mode (Collision yok veya basit hareket)
            targetPos += movement;
        }
    }

    if (m_GravityEnabled) {
        // Yercekimi ve Ziplama
        m_VerticalVelocity += m_Gravity * deltaTime;

        // Ziplama ve Tırmanma (Mantling)
        if (input->IsKeyDown(m_Bindings.jump)) {
            if (m_IsGrounded) {
                // Normal Ziplama
                m_VerticalVelocity = sqrt(m_JumpHeight * -2.0f * m_Gravity);
                m_IsGrounded = false;
            } else {
                // Havada veya yerde degilken ziplama tusuna basilirsa tirmanmayi dene
                // Mantling (Tirmanma) Mantigi
                glm::vec3 forward = m_Camera->GetFront();
                forward.y = 0.0f;
                if (glm::length(forward) > 0.01f) forward = glm::normalize(forward);

                // Onumuzdeki engeli bul
                float detectDist = 1.0f;
                glm::vec3 checkPos = currentPos + forward * detectDist;
                
                // Basitce onumuzde bir engel var mi diye bakiyoruz (CheckCollision ile)
                // Ancak engelin yuksekligini bulmamiz lazim.
                // Sahnedeki objeleri tarayip onumuzdeki en yakin ve uygun yukseklikteki objeyi bulalim.
                
                float bestMantleY = -1.0f;
                bool foundMantle = false;

                for (auto& entity : scene->GetEntities()) {
                    auto* collider = entity->GetComponent<BoxCollider>();
                    auto* transform = entity->GetComponent<Transform>();
                    
                    if (collider && transform) {
                        // Kutu sinirlarini dunya koordinatlarinda bul
                        glm::vec3 boxSize = collider->size * transform->scale;
                        glm::vec3 boxCenter = transform->position + collider->center * transform->scale; // Center offset varsa
                        
                        // AABB kontrolu (Basitlestirilmis)
                        glm::vec3 min = boxCenter - boxSize * 0.5f;
                        glm::vec3 max = boxCenter + boxSize * 0.5f;

                        // Oyuncu onundeki nokta kutunun icinde mi (XZ duzleminde)?
                        // Oyuncu genisligini de hesaba katarak biraz genisletelim
                        float buffer = 0.5f;
                        bool inX = checkPos.x >= min.x - buffer && checkPos.x <= max.x + buffer;
                        bool inZ = checkPos.z >= min.z - buffer && checkPos.z <= max.z + buffer;

                        if (inX && inZ) {
                            // Bu obje onumuzde. Yuksekligine bakalim.
                            float topY = max.y;
                            
                            // Tirmanilabilir yukseklik araligi:
                            // Ayak hizamizdan (currentPos.y - 1.8) biraz yukarida olmali
                            // Ve maksimum tirmanma yuksekligimizden (currentPos.y + 1.0 gibi) asagida olmali
                            // Veya kafa hizamiza yakin olabilir.
                            
                            float footY = currentPos.y - 1.8f;
                            // Ornegin: Ayaklarimizdan en az 0.5m yukarida, kafamizdan en fazla 1m yukarida
                            if (topY > footY + 0.5f && topY < currentPos.y + 0.5f) {
                                // Ustunde durulabilir mi? (Yani ustunde baska engel yok mu?)
                                // Hedef pozisyon: Kutunun ustu + oyuncu boyu
                                glm::vec3 mantleTarget = currentPos + forward * 0.5f; // Biraz ileri
                                mantleTarget.y = topY + 1.8f + 0.01f;
                                
                                // Hedef noktada carpisma var mi?
                                if (!CheckCollision(mantleTarget, scene, nullptr, 0.0f)) {
                                    if (topY > bestMantleY) {
                                        bestMantleY = topY;
                                        foundMantle = true;
                                    }
                                }
                            }
                        }
                    }
                }

                if (foundMantle) {
                    // Tirman! (Smooth)
                    m_IsMantling = true;
                    m_MantleStartPos = currentPos;
                    m_MantleTargetPos = currentPos + forward * 0.6f; // Hafif ileri
                    m_MantleTargetPos.y = bestMantleY + 1.8f + 0.01f; // Kutunun ustune
                    m_MantleTimer = 0.0f;
                    return; // Hemen basla
                }
            }
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
                targetPos.y = objectTopY + playerEyeHeight + 0.05f; // Epsilon increased to 0.05f to prevent clipping
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
            // Scale'i burada uygulamiyoruz, cunku BoxCollider size'i scale ile carpiyoruz
            // VEYA: Scale'i matrise ekleyip, box size'i local (1,1,1) gibi dusunebiliriz.
            // Mevcut yapida boxSize = collider->size * transform->scale yapiyorduk.
            // OBB icin: Inverse Transform yaparken Scale'i de tersine cevirmek gerekir.
            // Ancak Scale islemi AABB boyutunu degistirir, rotasyon ise eksenleri.
            // En temizi: Rotasyon ve Pozisyonu matrise koyalim. Scale'i AABB boyutuna yedirelim.
            
            glm::mat4 invModel = glm::inverse(model);

            // Helper lambda for OBB check
            auto CheckOBB = [&](const glm::vec3& localCenter, const glm::vec3& localSize) -> bool {
                // Kutu Boyutu (Scale dahil)
                glm::vec3 boxHalfSize = localSize * transform->scale * 0.5f;
                
                glm::vec3 expandedMin = localCenter - boxHalfSize;
                glm::vec3 expandedMax = localCenter + boxHalfSize;
                
                // Oyuncu Yaricapi (X ve Z)
                expandedMin.x -= playerRadius; expandedMax.x += playerRadius;
                expandedMin.z -= playerRadius; expandedMax.z += playerRadius;
                
                // Oyuncuyu tek bir nokta yerine, boyu boyunca birkac noktada test etmeliyiz.
                // Boylece egik duvarlara kafa veya ayak carpmasini yakalayabiliriz.
                // stepHeight: Bu yuksekligin altindaki engelleri yoksay (ayaklar girebilir)
                
                float checkStart = stepHeight + 0.01f; // Daha hassas kontrol (1cm)
                float checkEnd = playerHeight - 0.1f; // Kafanin biraz altina kadar
                
                // 3 Nokta kontrolu: Alt, Orta, Ust
                std::vector<float> checkHeights;
                checkHeights.push_back(checkStart);
                if (checkEnd > checkStart) {
                    checkHeights.push_back((checkStart + checkEnd) * 0.5f);
                    checkHeights.push_back(checkEnd);
                }

                for (float h : checkHeights) {
                    glm::vec4 testPos4 = invModel * glm::vec4(feetPos + glm::vec3(0, h, 0), 1.0f);
                    glm::vec3 testPos = glm::vec3(testPos4);
                    
                    bool overlapX = testPos.x >= expandedMin.x && testPos.x <= expandedMax.x;
                    bool overlapY = testPos.y >= expandedMin.y && testPos.y <= expandedMax.y;
                    bool overlapZ = testPos.z >= expandedMin.z && testPos.z <= expandedMax.z;
                    
                    if (overlapX && overlapY && overlapZ) {
                        // Zemin tespiti (outGroundHeight) - Sadece en alt nokta icin mantikli olabilir
                        // veya herhangi bir carpisma durumunda en yuksek noktayi bulmaya calisabiliriz.
                        if (outGroundHeight) {
                            float boxTopLocal = localCenter.y + boxHalfSize.y;
                            // Eger oyuncu kutunun ustundeyse (test noktasi kutunun ustune yakinsa)
                            if (testPos.y >= boxTopLocal - 0.1f) { // Biraz tolerans
                                glm::vec3 topPointLocal = testPos; 
                                topPointLocal.y = boxTopLocal;
                                glm::vec4 topPointWorld = model * glm::vec4(topPointLocal, 1.0f);
                                
                                if (topPointWorld.y > *outGroundHeight) {
                                    *outGroundHeight = topPointWorld.y;
                                }
                            }
                        }
                        return true;
                    }
                }
                return false;
            };

            bool hit = false;
            
            // Check main box
            if (glm::length(collider->size) > 0.01f) {
                if (CheckOBB(collider->center, collider->size)) hit = true;
            }

            // Check sub boxes - REMOVED (BoxCollider does not support subBoxes)
            /*
            for (const auto& box : collider->subBoxes) {
                if (CheckOBB(box.center, box.size)) hit = true;
            }
            */

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
