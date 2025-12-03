#include "Weapon.h"
#include "Projectile.h"
#include "../ecs/Entity.h"
#include "../input/Input.h"
#include "../rendering/Camera.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <algorithm>

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
        if (weapon->reloadTimer >= weapon->stats.reloadTime) {
            // Reload tamamlandı
            int ammoNeeded = weapon->stats.magSize - weapon->stats.currentMag;
            int ammoToReload = std::min(ammoNeeded, weapon->stats.totalAmmo);
            
            weapon->stats.currentMag += ammoToReload;
            weapon->stats.totalAmmo -= ammoToReload;
            
            weapon->isReloading = false;
            weapon->reloadTimer = 0.0f;
            
            std::cout << "Reload complete! Mag: " << weapon->stats.currentMag << "/" << weapon->stats.magSize 
                      << " | Total: " << weapon->stats.totalAmmo << std::endl;
        }
        return; // Reload sırasında ateş edilemez
    }



    // Manuel reload (R tuşu)
    if (input->IsKeyPressed(GLFW_KEY_R)) {
        if (weapon->stats.currentMag < weapon->stats.magSize && weapon->stats.totalAmmo > 0 && !weapon->isReloading) {
            Reload(weapon);
        }
    }

    // Silah değiştirme (1-4 tuşları)
    if (input->IsKeyPressed(GLFW_KEY_1)) weapon->SwitchWeapon(Weapon::WeaponType::Rifle);
    if (input->IsKeyPressed(GLFW_KEY_2)) weapon->SwitchWeapon(Weapon::WeaponType::Pistol);
    if (input->IsKeyPressed(GLFW_KEY_3)) weapon->SwitchWeapon(Weapon::WeaponType::Knife);
    if (input->IsKeyPressed(GLFW_KEY_4)) weapon->SwitchWeapon(Weapon::WeaponType::Grenade);
}

bool WeaponSystem::TryShoot(Weapon* weapon, Entity* entity, Scene* scene, Camera* camera, ProjectileSystem* projectileSystem) {
    if (!weapon) return false;

    // Ateş hızı kontrolü
    if (weapon->timeSinceLastShot < weapon->stats.fireRate) {
        return false;
    }

    // Mermi kontrolü (Bıçak hariç)
    if (weapon->type != Weapon::WeaponType::Knife) {
        if (weapon->stats.currentMag <= 0) {
            // Auto-reload (Bıçak ve Grenade hariç)
            if (weapon->type != Weapon::WeaponType::Grenade && weapon->stats.totalAmmo > 0 && !weapon->isReloading) {
                Reload(weapon);
            }
            return false;
        }
    }

    // Ateş et
    if (weapon->type != Weapon::WeaponType::Knife) {
        weapon->stats.currentMag--;
    }
    
    weapon->timeSinceLastShot = 0.0f;
    
    // Recoil ekle
    weapon->currentRecoil = weapon->stats.recoilAmount;

    // Projectile spawn et
    if (projectileSystem && scene && camera) {
        glm::vec3 spawnPos = camera->GetPosition() + camera->GetFront() * 0.5f; // Kamera önünde spawn
        glm::vec3 direction = camera->GetFront();
        
        if (weapon->type == Weapon::WeaponType::Knife) {
            // Knife Logic (Short Range Raycast)
            // TODO: Implement proper melee hit detection
            std::cout << "SWISH! Knife attack." << std::endl;
            // For now, just simulate a short range "bullet" that dies instantly
            // Or better, do a raycast here directly?
            // Let's stick to projectile system for consistency for now, but invisible/short lived?
            // Actually, let's just print for now as requested "Knife attack"
        } 
        else if (weapon->type == Weapon::WeaponType::Grenade) {
            projectileSystem->SpawnProjectile(
                scene,
                spawnPos,
                direction,
                weapon->stats.range, // Throw speed
                weapon->stats.damage,
                entity,
                Projectile::ProjectileType::Grenade
            );
        }
        else {
            // Guns
            projectileSystem->SpawnProjectile(
                scene,
                spawnPos,
                direction,
                weapon->stats.range * 2.0f, // Bullet Speed
                weapon->stats.damage,
                entity,
                Projectile::ProjectileType::Bullet
            );
        }
    }

    return true;
}

void WeaponSystem::Reload(Weapon* weapon) {
    if (!weapon || weapon->isReloading) return;
    
    if (weapon->stats.currentMag >= weapon->stats.magSize) {
        std::cout << "Magazine is full!" << std::endl;
        return;
    }
    
    if (weapon->stats.totalAmmo <= 0) {
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
