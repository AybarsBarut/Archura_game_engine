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

// Physics Helper: Apply Friction
void FPSController::ApplyFriction(float t) {
    if (!m_IsGrounded) return; // Havada surtunme yok (Source tarzi)

    glm::vec3 vel = m_Velocity;
    // Dikey hizi yoksay (Yercekimi ayri)
    vel.y = 0.0f;
    
    float speed = glm::length(vel);
    if (speed < 0.1f) return;

    float drop = 0.0f;
    
    // Konami code / Quake tarzi surtunme kontrolu
    float control = speed < m_StopSpeed ? m_StopSpeed : speed;
    drop += control * m_Friction * t;

    float newSpeed = speed - drop;
    if (newSpeed < 0.0f) newSpeed = 0.0f;
    
    if (speed > 0.0f) newSpeed /= speed;

    m_Velocity.x *= newSpeed;
    m_Velocity.z *= newSpeed;
}

// Physics Helper: Accelerate
void FPSController::Accelerate(glm::vec3 wishDir, float wishSpeed, float accel, float deltaTime) {
    float currentSpeed = glm::dot(m_Velocity, wishDir);
    float addSpeed = wishSpeed - currentSpeed;
    
    if (addSpeed <= 0.0f) return;
    
    float accelSpeed = accel * deltaTime * wishSpeed;
    if (accelSpeed > addSpeed) accelSpeed = addSpeed;
    
    m_Velocity.x += accelSpeed * wishDir.x;
    m_Velocity.y += accelSpeed * wishDir.y;
    m_Velocity.z += accelSpeed * wishDir.z;
}

// Physics Helper: Air Accelerate (Separate for clarity and future distinct logic)
void FPSController::AirAccelerate(glm::vec3 wishDir, float wishSpeed, float accel, float deltaTime) {
    float currentSpeed = glm::dot(m_Velocity, wishDir);
    float addSpeed = wishSpeed - currentSpeed;
    
    if (addSpeed <= 0.0f) return;
    
    float accelSpeed = accel * deltaTime * wishSpeed;
    if (accelSpeed > addSpeed) accelSpeed = addSpeed;
    
    m_Velocity.x += accelSpeed * wishDir.x;
    m_Velocity.y += accelSpeed * wishDir.y;
    m_Velocity.z += accelSpeed * wishDir.z;
}

void FPSController::HandleMovement(Input* input, Scene* scene, float deltaTime) {
    // 1. Durum Kontrolu (Grounded?)
    // Basit raycast kontrolu Update sonunda yapiliyor, burada flag kullaniyoruz.
    
    // 2. Giris Yonunu Hesapla (Wish Direction)
    m_IsRunning = input->IsKeyDown(m_Bindings.sprint);
    
    glm::vec3 forward = m_Camera->GetFront();
    glm::vec3 right = m_Camera->GetRight();
    
    forward.y = 0.0f;
    right.y = 0.0f;
    
    if (glm::length(forward) > 0.001f) forward = glm::normalize(forward);
    if (glm::length(right) > 0.001f) right = glm::normalize(right);
    
    glm::vec3 wishDir = glm::vec3(0.0f);
    if (input->IsKeyDown(m_Bindings.forward)) wishDir += forward;
    if (input->IsKeyDown(m_Bindings.backward)) wishDir -= forward;
    if (input->IsKeyDown(m_Bindings.left)) wishDir -= right;
    if (input->IsKeyDown(m_Bindings.right)) wishDir += right;
    
    if (glm::length(wishDir) > 0.001f) wishDir = glm::normalize(wishDir);
    
    
    if (glm::length(wishDir) > 0.001f) wishDir = glm::normalize(wishDir);
    
    // --- BUNNYHOPPING LOGIC ---
    // Ziplama kontrolunu surtunmeden ONCE yapmaliyiz.
    // Eger ziplama basarili olursa, m_IsGrounded false olur ve surtunme uygulanmaz.
    // Boylece hiz korunur.
    
    if (input->IsKeyDown(m_Bindings.jump) || input->IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) { // Sag tik da zipla olsun kolaylik icin
        if (m_IsGrounded) {
             m_Velocity.y = sqrt(m_JumpHeight * 2.0f * -m_Gravity); 
             m_IsGrounded = false;
        }
    }

    // 3. Fizik Uygula
    if (m_IsGrounded) {
        // Yerde Hareket
        ApplyFriction(deltaTime);
        
        float wishSpeed = m_IsRunning ? m_RunSpeed : m_WalkSpeed;
        Accelerate(wishDir, wishSpeed, m_Acceleration, deltaTime);
        
        // Yercekimini sifirla (Sadece gorsel degil, fiziksel reset)
        // CheckCollision kisminda anyway -2.0f yapiyoruz ama burada birikmesini onleyelim.
    } else {
        // Havada Hareket (Air Control)
        // AirSpeedCap (3.0f) ile sinirli ivmelenme, ama mevcut hiz korunur.
        AirAccelerate(wishDir, m_AirSpeedCap, m_AirAcceleration, deltaTime);
        
        // Yercekimi
        m_Velocity.y += m_Gravity * deltaTime;
    }
    
    // 4. Pozisyon Entegrasyonu ve Carpisma
    glm::vec3 currentPos = m_Camera->GetPosition();
    glm::vec3 targetPos = currentPos + m_Velocity * deltaTime;
    
    // X Hareketi (Slide)
    glm::vec3 nextPosX = currentPos;
    nextPosX.x = targetPos.x;
    if (CheckCollision(nextPosX, scene, nullptr, 0.3f)) {
        m_Velocity.x = 0.0f; // Duvara carpti, hizi kes (Slide daha karmasik ama simdilik durdurma)
        targetPos.x = currentPos.x;
    }
    
    // Z Hareketi (Slide)
    glm::vec3 nextPosZ = currentPos;
    nextPosZ.z = targetPos.z;
    // Y ekseni degismedi, hedef X degisti (targetPos.x yukarida guncellendi veya iptal oldu)
    nextPosZ.x = targetPos.x; 
    
    if (CheckCollision(nextPosZ, scene, nullptr, 0.3f)) {
         m_Velocity.z = 0.0f;
         targetPos.z = currentPos.z;
    }
    
    // Y Hareketi (Ground / Ceiling)
    glm::vec3 nextPosY = targetPos;
    // X ve Z guncellendi
    
    float objectTopY = 0.0f;
    bool hitY = CheckCollision(nextPosY, scene, &objectTopY, 0.0f);
    
    // Yere yakinlik kontrolu (Yer tespiti)
    float playerEyeHeight = 1.8f;
    
    if (hitY) {
        if (m_Velocity.y <= 0.0f && (targetPos.y - playerEyeHeight) >= (objectTopY - 0.5f)) {
             // Landed
             targetPos.y = objectTopY + playerEyeHeight + 0.001f;
             m_Velocity.y = -2.0f; // Stick to ground
             m_IsGrounded = true;
        } else if (m_Velocity.y > 0.0f) {
             // Head bump
             m_Velocity.y = 0.0f;
             // targetPos.y ayni kalsin veya asagi itilsin? 
             // Simdilik carpisma oncesi pozisyona cekelim Y'yi
             targetPos.y = currentPos.y; 
        }
    } else {
        // Havada (En azindan su anlik carpmadi)
        // Ama onceki karede yerdeydik, simdi boslukta miyiz?
        float groundCheckY = targetPos.y - 0.2f; // Biraz asagi bak
        // Eger asagi dogru hizimiz varsa ve yere cok yakinsak 'snap' yapabiliriz (Merdiven inisi vb)
        // Simdilik basit: Carpmadiysa havadadir.
        
        // Yere cok yakin mi kontrol et (Eger IsGrounded idiysek ve bosluga dusmediysek)
        // Bu kisim karmasik, o yuzden simple logic:
        // Eger hitY yoksa ve Y hizi asagi dogruysa veya 0 ise -> falling
        if (m_IsGrounded && m_Velocity.y <= 0.0f) {
             // Yoklama atis (cast down)
             glm::vec3 checkPos = targetPos;
             checkPos.y -= 0.5f; 
             float gHeight = 0.0f;
             if (CheckCollision(checkPos, scene, &gHeight, 0.0f)) {
                 // Hala zemindeyiz (egim veya kucuk cukur)
                 if (targetPos.y - playerEyeHeight <= gHeight + 0.5f) {
                      targetPos.y = gHeight + playerEyeHeight + 0.001f;
                      m_Velocity.y = -2.0f;
                 } else {
                      m_IsGrounded = false;
                 }
             } else {
                 m_IsGrounded = false;
             }
        } else {
             m_IsGrounded = false;
        }
        
    }
    
    // Kamerayi Guncelle
    m_Camera->SetPosition(targetPos);

    // FOV kontrolu (fare tekerlegi) (Eski koddan)
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
