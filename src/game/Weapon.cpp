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

    // Zamanlayicilari guncelle
    weapon->timeSinceLastShot += deltaTime;

    // Sarjor degistirme durumu
    if (weapon->isReloading) {
        weapon->reloadTimer += deltaTime;
        if (weapon->reloadTimer >= weapon->stats.reloadTime) {
            // Sarjor degistirme tamamlandi
            int ammoNeeded = weapon->stats.magSize - weapon->stats.currentMag;
            int ammoToReload = std::min(ammoNeeded, weapon->stats.totalAmmo);
            
            weapon->stats.currentMag += ammoToReload;
            weapon->stats.totalAmmo -= ammoToReload;
            
            weapon->isReloading = false;
            weapon->reloadTimer = 0.0f;
            
            // std::cout << "Reload complete! Mag: " << weapon->stats.currentMag << "/" << weapon->stats.magSize << std::endl; 
                      << " | Total: " << weapon->stats.totalAmmo << std::endl;
        }
        return; // Sarjor degistirme sirasinda ates edilemez
    }



    // Manuel sarjor degistirme (R tusu)
    if (input->IsKeyPressed(GLFW_KEY_R)) {
        if (weapon->stats.currentMag < weapon->stats.magSize && weapon->stats.totalAmmo > 0 && !weapon->isReloading) {
            Reload(weapon);
        }
    }

    // Silah degistirme (1-4 tuslari)
    if (input->IsKeyPressed(GLFW_KEY_1)) weapon->SwitchWeapon(Weapon::WeaponType::Rifle);
    if (input->IsKeyPressed(GLFW_KEY_2)) weapon->SwitchWeapon(Weapon::WeaponType::Pistol);
    if (input->IsKeyPressed(GLFW_KEY_3)) weapon->SwitchWeapon(Weapon::WeaponType::Knife);
    if (input->IsKeyPressed(GLFW_KEY_4)) weapon->SwitchWeapon(Weapon::WeaponType::Grenade);
}

bool WeaponSystem::TryShoot(Weapon* weapon, Entity* entity, Scene* scene, Camera* camera, ProjectileSystem* projectileSystem) {
    if (!weapon) return false;

    // Ates hizi kontrolu
    if (weapon->timeSinceLastShot < weapon->stats.fireRate) {
        return false;
    }

    // Mermi kontrolu (Bicak haric)
    if (weapon->type != Weapon::WeaponType::Knife) {
        if (weapon->stats.currentMag <= 0) {
            // Otomatik sarjor degistirme (Bicak ve El Bombasi haric)
            if (weapon->type != Weapon::WeaponType::Grenade && weapon->stats.totalAmmo > 0 && !weapon->isReloading) {
                Reload(weapon);
            }
            return false;
        }
    }

    // Ates et
    if (weapon->type != Weapon::WeaponType::Knife) {
        weapon->stats.currentMag--;
    }
    
    weapon->timeSinceLastShot = 0.0f;
    
    // Geri tepme ekle
    weapon->currentRecoil = weapon->stats.recoilAmount;

    // Mermi olustur
    if (projectileSystem && scene && camera) {
        glm::vec3 spawnPos = camera->GetPosition() + camera->GetFront() * 0.5f; // Kamera onunde olustur
        glm::vec3 direction = camera->GetFront();
        
        if (weapon->type == Weapon::WeaponType::Knife) {
            // Bicak Mantigi (Kisa Menzilli Isin Izleme)
            // YAPILACAK: Duzgun yakin dovus vurus tespiti uygula
            // std::cout << "SWISH! Knife attack." << std::endl;
            // Simdilik sadece aninda yok olan kisa menzilli bir "mermi" simule et
            // Veya daha iyisi, burada dogrudan bir isin izleme mi yapilmali?
            // Simdilik tutarlilik icin mermi sistemine sadik kalalim, ama gorunmez/kisa omurlu?
            // Aslinda, simdilik sadece istendigi gibi "Bicak saldirisi" yazdiralim
        } 
        else if (weapon->type == Weapon::WeaponType::Grenade) {
            projectileSystem->SpawnProjectile(
                scene,
                spawnPos,
                direction,
                weapon->stats.range, // Firlatma hizi
                weapon->stats.damage,
                entity,
                Projectile::ProjectileType::Grenade
            );
        }
        else {
            // Silahlar
            projectileSystem->SpawnProjectile(
                scene,
                spawnPos,
                direction,
                weapon->stats.range * 2.0f, // Mermi Hizi
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
        // std::cout << "Magazine is full!" << std::endl;
        return;
    }
    
    if (weapon->stats.totalAmmo <= 0) {
        // std::cout << "No ammo left!" << std::endl;
        return;
    }

    weapon->isReloading = true;
    weapon->reloadTimer = 0.0f;
    // std::cout << "Reloading..." << std::endl;
}

void WeaponSystem::UpdateRecoil(Weapon* weapon, Camera* camera, float deltaTime) {
    if (!weapon || !camera) return;

    // Geri tepme toparlanmasi
    if (weapon->currentRecoil > 0.0f) {
        weapon->currentRecoil -= deltaTime * 10.0f; // Toparlanma hizi
        weapon->currentRecoil = std::max(0.0f, weapon->currentRecoil);
    }
}

void WeaponSystem::ApplyRecoil(Weapon* weapon, Camera* camera) {
    if (!weapon || !camera) return;

    // Kamerayi yukari hareket ettir (geri tepme simulasyonu)
    float recoilPitch = weapon->currentRecoil * -1.0f;
    camera->ProcessMouseMovement(0.0f, recoilPitch);
}

} // namespace Archura
