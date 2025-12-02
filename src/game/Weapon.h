#pragma once

#include "../ecs/Component.h"
#include <glm/glm.hpp>

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
        Pistol,
        Rifle,
        Shotgun,
        Sniper
    };

    WeaponType type = WeaponType::Pistol;
    
    // Silah istatistikleri
    float damage = 25.0f;
    float fireRate = 0.2f; // Saniye cinsinden atis araligi
    float range = 100.0f;
    float accuracy = 0.95f; // 0-1 arasi
    
    // Mermi
    int magSize = 12;
    int currentMag = 12;
    int totalAmmo = 60;
    float reloadTime = 1.5f;
    
    // State
    float timeSinceLastShot = 0.0f;
    bool isReloading = false;
    float reloadTimer = 0.0f;
    
    // Recoil
    float recoilAmount = 2.0f;
    float currentRecoil = 0.0f;
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
