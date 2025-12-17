#include "FPSController.h"
#include "../input/Input.h"
#include "../ecs/Entity.h"
#include "../ecs/Component.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../game/Weapon.h"
#include "../game/Projectile.h"
#include <algorithm>

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

void FPSController::Update(Input* input, Scene* scene, float deltaTime, ProjectileSystem* projectileSystem) {
    HandleMovement(input, scene, deltaTime);
    HandleMouseLook(input, deltaTime);

    // --- GERİ TEPME İYİLEŞTİRMESİ ---
    if (glm::length(m_CurrentRecoil) > 0.001f) {
        glm::vec3 recovery = m_CurrentRecoil * m_RecoilReturnSpeed * deltaTime;
        
        // Aşırı iyileştirme yapma (over-recover)
        if (glm::length(recovery) > glm::length(m_CurrentRecoil)) {
            recovery = m_CurrentRecoil;
        }

        m_CurrentRecoil -= recovery;
        
        // Kamerayı düzelt (aşağı eğ)
        // Not: m_CurrentRecoil pozitiftir (yukarı vuruş), bu yüzden çıkararak (aşağı bakarak) telafi ediyoruz.
        // Ama ProcessMouseMovement (x, y) alır ve y eğimdir (pitch).
        // Eğer geri tepme için +Y (Yukarı) eklediysek, düzeltmek için -Y ekleriz.
        m_Camera->ProcessMouseMovement(-recovery.y, -recovery.x, true);
    }

    // --- ATIŞ MANTIĞI ---
    if (input->IsMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
        // Oyuncuyu Bul (Her kare verimsiz arama, ama demo için uygun)
        Entity* player = nullptr;
        for(auto& e : scene->GetEntities()) {
            if(e->GetName() == "Player") { player = e.get(); break; }
        }

        if (player && projectileSystem) {
            auto* weapon = player->GetComponent<Weapon>();
            if (weapon) {
                // Genel sistem sarmalayıcısını örneklendir
                WeaponSystem ws;
                if (ws.TryShoot(weapon, player, scene, m_Camera, projectileSystem)) {
                    // Geri Tepme Uygula
                    // Eğim (X ekseni dönüşü) genellikle Y fare hareketidir
                    // Geri tepme YUKARI teper, bu yüzden fare YUKARI (+Y) hareket etmiş gibi davranıyoruz
                    // Rastgele yatay sallantı
                    float rX = ((rand() % 100) / 100.0f - 0.5f) * weapon->stats.recoilAmount * 0.5f;
                    float rY = weapon->stats.recoilAmount; 

                    AddRecoil(glm::vec3(rY, rX, 0.0f)); 
                }
            }
        }
    }
}

void FPSController::HandleMovement(Input* input, Scene* scene, float deltaTime) {
    // Mantling (Tirmanma) Durumu - IPTAL EDILDI (User Request)
    /*
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
    */

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
                // Havada veya yerde degilken ziplama tusuna basilirsa tirmanmayi dene - IPTAL EDILDI
                /*
                // Mantling (Tirmanma) Mantigi
                glm::vec3 forward = m_Camera->GetFront();
                forward.y = 0.0f;
                if (glm::length(forward) > 0.01f) forward = glm::normalize(forward);

                // ... (Kodun geri kalanı yorum satırına alındı) ...
                
                if (foundMantle) {
                    m_IsMantling = true;
                    // ...
                    return; 
                }
                */
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
            // Eger asagi dogru dusuyorsak (veya duruyorsak) YERDEDIR
            if (m_VerticalVelocity <= 0.0f && (targetPos.y - playerEyeHeight) >= (objectTopY - 0.5f)) {
                
                // Titreşim Düzeltmesi: Epsilon azaltıldı ve yapışkan yerçekimi uygulandı
                // 0.05f yerine 0.001f kullaniyoruz, boylece havada kalmaz.
                float groundSnapY = objectTopY + playerEyeHeight + 0.001f; 
                
                targetPos.y = groundSnapY;
                
                // Yere yapış: Hafif eksi hız uygulayarak sonraki karede de "yere çarpma" kontrolünü garantile
                m_VerticalVelocity = -2.0f; 
                m_IsGrounded = true;
            }
            else if (m_VerticalVelocity > 0.0f) {
                // Kafayi carpti
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
            if (collider->isTrigger) continue;

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

            // OBB kontrolü için yardımcı lambda
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
            
            // Ana kutuyu kontrol et
            if (glm::length(collider->size) > 0.01f) {
                if (CheckOBB(collider->center, collider->size)) hit = true;
            }

            // Alt kutuları kontrol et - KALDIRILDI (BoxCollider alt kutuları desteklemez)
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

    // Sol tik ile imleci kilitleme mantigi Application.cpp'ye tasindi.
    // Artik burada UI durumu kontrol edilmeden kilitlenmeyecek.
    
    // ESC ile imleci serbest birak (Gecici cikis)
    // Not: Ana dongude ESC cikis yapiyor olabilir, bunu kontrol etmeliyiz.
    // Simdilik sadece kilitli degilse serbest birakma mantigi kalsin.
}

void FPSController::AddRecoil(const glm::vec3& recoil) {
    // Anlık vuruş ekle
    m_CurrentRecoil += recoil;

    // Kameraya hemen uygula (Yukarı Vur)
    m_Camera->ProcessMouseMovement(recoil.y, recoil.x, true);
}

void FPSController::ResetRecoil() {
    m_CurrentRecoil = glm::vec3(0.0f);
}

} // namespace Archura
