#include "Weapon.h"
#include "Projectile.h"
#include "../ecs/Entity.h"
#include "../input/Input.h"
#include "../rendering/Camera.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

namespace Archura {

void WeaponSystem::Update(Entity* entity, Input* input, Scene* scene, Camera* camera, ProjectileSystem* projectileSystem, float deltaTime) {
    if (!entity) return;

    auto* weapon = entity->GetComponent<Weapon>();
    if (!weapon) return;

    // Timer'ları güncelle
    weapon->timeSinceLastShot += deltaTime;

    // Reload durumu
    if (weapon->isReloading) {
        weapon->reloadTimer += deltaTime;
        if (weapon->reloadTimer >= weapon->reloadTime) {
            // Reload tamamlandı
            int ammoNeeded = weapon->magSize - weapon->currentMag;
            int ammoToReload = std::min(ammoNeeded, weapon->totalAmmo);
            
            weapon->currentMag += ammoToReload;
            weapon->totalAmmo -= ammoToReload;
            
            weapon->isReloading = false;
            weapon->reloadTimer = 0.0f;
            
            std::cout << "Reload complete! Mag: " << weapon->currentMag << "/" << weapon->magSize 
                      << " | Total: " << weapon->totalAmmo << std::endl;
        }
        return; // Reload sırasında ateş edilemez
    }



    // Manuel reload (R tuşu)
    if (input->IsKeyPressed(GLFW_KEY_R)) {
        if (weapon->currentMag < weapon->magSize && weapon->totalAmmo > 0 && !weapon->isReloading) {
            Reload(weapon);
        }
    }

    // Silah değiştirme (1-4 tuşları) - örnek
    if (input->IsKeyPressed(GLFW_KEY_1)) weapon->type = Weapon::WeaponType::Pistol;
    if (input->IsKeyPressed(GLFW_KEY_2)) weapon->type = Weapon::WeaponType::Rifle;
    if (input->IsKeyPressed(GLFW_KEY_3)) weapon->type = Weapon::WeaponType::Shotgun;
    if (input->IsKeyPressed(GLFW_KEY_4)) weapon->type = Weapon::WeaponType::Sniper;
}

bool WeaponSystem::TryShoot(Weapon* weapon, Entity* entity, Scene* scene, Camera* camera, ProjectileSystem* projectileSystem) {
    if (!weapon) return false;

    // Ateş hızı kontrolü
    if (weapon->timeSinceLastShot < weapon->fireRate) {
        return false;
    }

    // Mermi kontrolü
    if (weapon->currentMag <= 0) {
        // Auto-reload
        if (weapon->totalAmmo > 0 && !weapon->isReloading) {
            Reload(weapon);
        }
        return false;
    }

    // Ateş et
    weapon->currentMag--;
    weapon->timeSinceLastShot = 0.0f;
    
    // Recoil ekle
    weapon->currentRecoil = weapon->recoilAmount;

    // Projectile spawn et
    if (projectileSystem && scene && camera) {
        glm::vec3 spawnPos = camera->GetPosition() + camera->GetFront() * 0.5f; // Kamera önünde spawn
        glm::vec3 direction = camera->GetFront();
        
        projectileSystem->SpawnProjectile(
            scene,
            spawnPos,
            direction,
            weapon->range * 2.0f, // Speed (range'e göre ayarla)
            weapon->damage,
            entity
        );
    }

    return true;
}

void WeaponSystem::Reload(Weapon* weapon) {
    if (!weapon || weapon->isReloading) return;
    
    if (weapon->currentMag >= weapon->magSize) {
        std::cout << "Magazine is full!" << std::endl;
        return;
    }
    
    if (weapon->totalAmmo <= 0) {
        std::cout << "No ammo left!" << std::endl;
        return;
    }

    weapon->isReloading = true;
    weapon->reloadTimer = 0.0f;
    std::cout << "Reloading..." << std::endl;
}

void WeaponSystem::UpdateRecoil(Weapon* weapon, Camera* camera, float deltaTime) {
    if (!weapon || !camera) return;

    // Recoil recovery
    if (weapon->currentRecoil > 0.0f) {
        weapon->currentRecoil -= deltaTime * 10.0f; // Recovery hızı
        weapon->currentRecoil = std::max(0.0f, weapon->currentRecoil);
    }
}

void WeaponSystem::ApplyRecoil(Weapon* weapon, Camera* camera) {
    if (!weapon || !camera) return;

    // Kamerayı yukarı hareket ettir (recoil simülasyonu)
    float recoilPitch = weapon->currentRecoil * -1.0f;
    camera->ProcessMouseMovement(0.0f, recoilPitch);
}

} // namespace Archura
