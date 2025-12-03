#pragma once

#include "../ecs/Component.h"
#include <glm/glm.hpp>
#include <map>

namespace Archura {

class Entity;
class Input;
class Scene;
class Camera;
class ProjectileSystem;

/**
 * @brief Weapon component - Silah ozellikleri
 */
struct Weapon : public Component {
    enum class WeaponType {
        Rifle = 0,
        Pistol = 1,
        Knife = 2,
        Grenade = 3,
        Shotgun = 4,
        Sniper = 5
    };

    struct WeaponStats {
        float damage = 25.0f;
        float fireRate = 0.2f;
        float range = 100.0f;
        int magSize = 12;
        int currentMag = 12;
        int totalAmmo = 60;
        float reloadTime = 1.5f;
        float recoilAmount = 2.0f;
        bool isAutomatic = true;
    };

    WeaponType type = WeaponType::Rifle;
    
    // Inventory: Map WeaponType to Stats
    std::map<WeaponType, WeaponStats> inventory;

    // Active Stats (copied from inventory for easy access)
    WeaponStats stats;

    // State
    float timeSinceLastShot = 0.0f;
    bool isReloading = false;
    float reloadTimer = 0.0f;
    float currentRecoil = 0.0f;

    // Helper to initialize inventory
    void InitInventory() {
        // Rifle
        inventory[WeaponType::Rifle] = { 30.0f, 0.1f, 100.0f, 30, 30, 120, 2.0f, 2.0f, true };
        // Pistol
        inventory[WeaponType::Pistol] = { 20.0f, 0.2f, 50.0f, 12, 12, 48, 1.5f, 1.0f, false };
        // Knife
        inventory[WeaponType::Knife] = { 50.0f, 0.5f, 2.0f, 0, 0, 0, 0.0f, 0.0f, false };
        // Grenade
        inventory[WeaponType::Grenade] = { 100.0f, 1.0f, 20.0f, 1, 1, 5, 0.0f, 0.0f, false };
        
        // Set default
        SwitchWeapon(WeaponType::Rifle);
    }

    void SwitchWeapon(WeaponType newType) {
        // Save current stats to inventory
        if (inventory.find(type) != inventory.end()) {
            inventory[type] = stats;
        }

        // Load new stats
        if (inventory.find(newType) != inventory.end()) {
            type = newType;
            stats = inventory[newType];
            isReloading = false;
            reloadTimer = 0.0f;
        }
    }
};

/**
 * @brief WeaponSystem - Silah mekanigi yonetimi
 */
class WeaponSystem {
public:
    WeaponSystem() = default;
    ~WeaponSystem() = default;

    void Update(Entity* entity, Input* input, Scene* scene, Camera* camera, ProjectileSystem* projectileSystem, float deltaTime);
    
    // Silah aksiyonlari
    bool TryShoot(Weapon* weapon, Entity* entity, Scene* scene, Camera* camera, ProjectileSystem* projectileSystem);
    void Reload(Weapon* weapon);
    void UpdateRecoil(Weapon* weapon, Camera* camera, float deltaTime);

private:
    void ApplyRecoil(Weapon* weapon, Camera* camera);
};

} // namespace Archura
