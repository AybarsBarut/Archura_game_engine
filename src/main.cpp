#include "core/Engine.h"
#include "core/Window.h"
#include "input/Input.h"
#include "rendering/Renderer.h"
#include <GLFW/glfw3.h>
#include "core/ResourceManager.h"
#include "ecs/Entity.h"
#include "ecs/Component.h"
#include "rendering/Mesh.h"
#include "rendering/Camera.h"
#include "rendering/HUDRenderer.h"
#include "rendering/Skybox.h"
#include "game/FPSController.h"
#include "game/RenderSystem.h"
#include "game/Weapon.h"
#include "game/Projectile.h"
#include "editor/Editor.h"
#include <iostream>
#include <memory>

using namespace Archura;

int main() {
    // Motor konfigurasyonu
    Engine::EngineConfig config;
    config.windowTitle = "Archura FPS Engine - Demo";
    config.windowWidth = 1920;
    config.windowHeight = 1080;
    config.vsync = false; // VSync kapali = sinirsiz FPS (120+ hedef)
    config.fullscreen = false;
    config.editorMode = false; // Simdilik editor UI kapali

    // Motoru baslat
    auto& engine = Engine::Get();
    if (!engine.Init(config)) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return -1;
    }

    std::cout << "\n=== Creating Demo Scene ===" << std::endl;

    // Sahne olustur
    Scene scene("Demo Scene");

    // Kamera ve FPS kontrolcusu
    Camera camera(glm::vec3(0.0f, 2.0f, 10.0f));
    camera.SetMovementSpeed(5.0f);
    FPSController fpsController(&camera);

    // Render system
    RenderSystem renderSystem(&camera);
    renderSystem.Init(&scene);

    // Skybox
    Skybox skybox;
    skybox.Init();

    // HUD Renderer
    HUDRenderer hudRenderer;
    if (!hudRenderer.Init()) {
        std::cerr << "Failed to initialize HUD renderer!" << std::endl;
        return -1;
    }
    hudRenderer.SetScreenSize(1920.0f, 1080.0f);

    // Editor (ImGui)
    Editor editor;
    if (!editor.Init(engine.GetWindow())) {
        std::cerr << "Failed to initialize Editor!" << std::endl;
        return -1;
    }
    editor.SetEnabled(true); // Editor aktif

    // Oyuncu varligi (silah tasiyicisi)
    Entity* player = scene.CreateEntity("Player");
    auto* weapon = player->AddComponent<Weapon>();
    weapon->type = Weapon::WeaponType::Rifle;
    weapon->damage = 30.0f;
    weapon->fireRate = 0.1f;
    weapon->magSize = 30;
    weapon->currentMag = 30;
    weapon->totalAmmo = 120;
    weapon->reloadTime = 2.0f;

    // Silah sistemi
    WeaponSystem weaponSystem;

    // Mermi sistemi
    ProjectileSystem projectileSystem;

    // Oyuncu cani (demo)
    float playerHealth = 100.0f;
    float maxHealth = 100.0f;

    // ==================== Demo Varliklar ====================
    
    // ==================== Harita Olusturma ====================
    
    // Zemin (Platform)
    Entity* ground = scene.CreateEntity("Ground");
    auto* groundMesh = ground->AddComponent<MeshRenderer>();
    groundMesh->mesh = Mesh::CreatePlane(100.0f, 100.0f);
    groundMesh->color = glm::vec3(0.2f, 0.25f, 0.2f); // Koyu gri-yesil zemin
    auto* groundTransform = ground->GetComponent<Transform>();
    groundTransform->position = glm::vec3(0.0f, 0.0f, 0.0f);

    // Duvar Materyali (Gri Beton)
    glm::vec3 wallColor = glm::vec3(0.5f, 0.5f, 0.55f);
    glm::vec3 obstacleColor = glm::vec3(0.6f, 0.4f, 0.3f); // Kahverengi kutular

    // Harita Sinirlari (Dis Duvarlar)
    float mapSize = 50.0f; // 100x100 zemin icin -50 ile +50 arasi
    float wallHeight = 5.0f;
    float wallThickness = 2.0f;

    struct WallDef {
        glm::vec3 pos;
        glm::vec3 scale;
        glm::vec3 color;
    };

    std::vector<WallDef> walls;

    // 4 Kenar Duvar
    walls.push_back({ glm::vec3(0, wallHeight/2, -mapSize), glm::vec3(mapSize*2, wallHeight, wallThickness), wallColor }); // Kuzey
    walls.push_back({ glm::vec3(0, wallHeight/2, mapSize), glm::vec3(mapSize*2, wallHeight, wallThickness), wallColor }); // Guney
    walls.push_back({ glm::vec3(-mapSize, wallHeight/2, 0), glm::vec3(wallThickness, wallHeight, mapSize*2), wallColor }); // Bati
    walls.push_back({ glm::vec3(mapSize, wallHeight/2, 0), glm::vec3(wallThickness, wallHeight, mapSize*2), wallColor }); // Dogu

    // Ic Duvarlar ve Engeller (Labirentimsi Yapi)
    // Sol taraf siperleri
    walls.push_back({ glm::vec3(-15, 2, -15), glm::vec3(10, 4, 2), wallColor });
    walls.push_back({ glm::vec3(-25, 2, -5), glm::vec3(2, 4, 15), wallColor });

    // Sag taraf siperleri
    walls.push_back({ glm::vec3(15, 2, 15), glm::vec3(10, 4, 2), wallColor });
    walls.push_back({ glm::vec3(25, 2, 5), glm::vec3(2, 4, 15), wallColor });

    // Orta Alan (Arena)
    walls.push_back({ glm::vec3(0, 1.5f, 0), glm::vec3(5, 3, 5), obstacleColor }); // Merkez kutu
    walls.push_back({ glm::vec3(-8, 1, 8), glm::vec3(3, 2, 3), obstacleColor });
    walls.push_back({ glm::vec3(8, 1, -8), glm::vec3(3, 2, 3), obstacleColor });

    // Rastgele dagilmis kucuk siperler
    walls.push_back({ glm::vec3(0, 1.5f, 25), glm::vec3(8, 3, 1), wallColor });
    walls.push_back({ glm::vec3(0, 1.5f, -25), glm::vec3(8, 3, 1), wallColor });
    walls.push_back({ glm::vec3(25, 1.5f, 0), glm::vec3(1, 3, 8), wallColor });
    walls.push_back({ glm::vec3(-25, 1.5f, 0), glm::vec3(1, 3, 8), wallColor });

    // Duvarlari Olustur
    int wallCount = 0;
    for (const auto& def : walls) {
        Entity* wall = scene.CreateEntity("Wall_" + std::to_string(wallCount++));
        
        // Mesh
        auto* mesh = wall->AddComponent<MeshRenderer>();
        mesh->mesh = Mesh::CreateCube(1.0f); // Temel kup, scale ile buyutecegiz
        mesh->color = def.color;

        // Transform
        auto* transform = wall->GetComponent<Transform>();
        transform->position = def.pos;
        transform->scale = def.scale;

        // Collider
        auto* collider = wall->AddComponent<BoxCollider>();
        collider->size = glm::vec3(1.0f); // Mesh boyutu 1x1x1 oldugu icin collider da 1x1x1 (scale ile carpilacak)
    }

    std::cout << "Scene created with " << scene.GetEntities().size() << " entities." << std::endl;
    std::cout << "\n=== Controls ===" << std::endl;
    std::cout << "  WASD: Move" << std::endl;
    std::cout << "  Q/E: Up/Down" << std::endl;
    std::cout << "  Right Mouse + Move: Look Around" << std::endl;
    std::cout << "  Left Mouse: Shoot" << std::endl;
    std::cout << "  R: Reload" << std::endl;
    std::cout << "  1-4: Switch Weapon Type" << std::endl;
    std::cout << "  Shift: Sprint" << std::endl;
    std::cout << "  Mouse Scroll: Zoom (FOV)" << std::endl;
    std::cout << "  TAB: Toggle Editor UI" << std::endl;
    std::cout << "  ESC: Exit" << std::endl;
    std::cout << "==================\n" << std::endl;

    // Ana oyun dongusu - Engine.Run() yerine manuel dongu
    auto* window = engine.GetWindow();
    auto* input = engine.GetInput();
    auto* renderer = engine.GetRenderer();

    while (!window->ShouldClose()) {
        float deltaTime = window->GetDeltaTime();
        
        // Giris guncellemesi
        input->Update();

        // TAB tusu ile Editor ac/kapa
        if (input->IsKeyPressed(GLFW_KEY_TAB)) {
            bool newState = !editor.IsEnabled();
            editor.SetEnabled(newState);
            
            if (newState) {
                input->SetCursorMode(GLFW_CURSOR_NORMAL);
            } else {
                input->SetCursorMode(GLFW_CURSOR_DISABLED);
            }
            
            std::cout << "Editor: " << (newState ? "ON" : "OFF") << std::endl;
        }

        // FPS kontrolcusu guncellemesi
        fpsController.Update(input, &scene, deltaTime);

        // Silah sistemi guncellemesi (mermi olusturma ile)
        weaponSystem.Update(player, input, &scene, &camera, &projectileSystem, deltaTime);
        weaponSystem.UpdateRecoil(weapon, &camera, deltaTime);

        // Mermi sistemi guncellemesi
        projectileSystem.Update(&scene, deltaTime);

        // 3D sahne cizimi
        renderer->BeginFrame();
        
        // Skybox Cizimi
        skybox.Draw(camera, (float)config.windowWidth / (float)config.windowHeight);

        renderSystem.Update(deltaTime);
        
        // Editor Arayuzu (ImGui) - HUD'dan once
        editor.BeginFrame();
        editor.Update(&scene, deltaTime, window->GetFPS());
        
        // HUD Cizimi (2D katman)
        hudRenderer.BeginHUD();
        
        // Nisangah
        hudRenderer.DrawCrosshair(20.0f, glm::vec4(0.0f, 1.0f, 0.0f, 0.8f));
        
        // Mermi sayaci (sag alt)
        int totalLoadedAmmo = weapon->currentMag + weapon->totalAmmo;
        hudRenderer.DrawAmmoCounter(weapon->currentMag, weapon->magSize, 1680.0f, 50.0f);
        
        // Can bari (sol alt)
        hudRenderer.DrawHealthBar(playerHealth, maxHealth, 40.0f, 50.0f, 300.0f, 30.0f);
        
        hudRenderer.EndHUD();
        
        editor.EndFrame();
        
        renderer->EndFrame();

        // Pencere guncellemesi
        window->Update();

        // ESC tusu ile cikis veya cursor serbest birakma
        if (input->IsKeyPressed(GLFW_KEY_ESCAPE)) {
            if (input->IsCursorLocked()) {
                input->SetCursorMode(GLFW_CURSOR_NORMAL);
            } else {
                break; // Zaten serbestse cikis yap
            }
        }

        // FPS bilgisini goster (1 saniyede bir)
        static float fpsTimer = 0.0f;
        fpsTimer += deltaTime;
        if (fpsTimer >= 1.0f) {
            std::cout << "FPS: " << window->GetFPS() 
                      << " | Frame Time: " << (deltaTime * 1000.0f) << "ms"
                      << " | Camera Pos: (" << camera.GetPosition().x << ", " 
                      << camera.GetPosition().y << ", " << camera.GetPosition().z << ")"
                      << std::endl;
            fpsTimer = 0.0f;
        }
    }

    // Cleanup
    editor.Shutdown();
    hudRenderer.Shutdown();
    renderSystem.Shutdown();
    
    // Mesh'leri temizle
    for (auto& entity : scene.GetEntities()) {
        auto* meshRenderer = entity->GetComponent<MeshRenderer>();
        if (meshRenderer && meshRenderer->mesh) {
            delete meshRenderer->mesh;
            meshRenderer->mesh = nullptr;
        }
    }

    engine.Shutdown();

    return 0;
}
