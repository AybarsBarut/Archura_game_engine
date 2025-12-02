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
    
    // Zemin (Platform)
    Entity* ground = scene.CreateEntity("Ground");
    auto* groundMesh = ground->AddComponent<MeshRenderer>();
    groundMesh->mesh = Mesh::CreatePlane(20.0f, 20.0f);
    groundMesh->color = glm::vec3(0.3f, 0.5f, 0.3f); // Yesilimsi
    auto* groundTransform = ground->GetComponent<Transform>();
    groundTransform->position = glm::vec3(0.0f, 0.0f, 0.0f);

    // Merkez kup (Oyuncuya referans noktasi)
    Entity* centerCube = scene.CreateEntity("Center Cube");
    auto* centerCubeMesh = centerCube->AddComponent<MeshRenderer>();
    centerCubeMesh->mesh = Mesh::CreateCube(2.0f);
    centerCubeMesh->color = glm::vec3(1.0f, 0.3f, 0.3f); // Kirmizi
    auto* centerCubeTransform = centerCube->GetComponent<Transform>();
    centerCubeTransform->position = glm::vec3(0.0f, 1.0f, 0.0f);

    // Küreler (cesitli pozisyonlarda)
    for (int i = 0; i < 5; ++i) {
        float angle = (i / 5.0f) * 2.0f * 3.14159f;
        float radius = 8.0f;
        
        Entity* sphere = scene.CreateEntity("Sphere " + std::to_string(i));
        auto* sphereMesh = sphere->AddComponent<MeshRenderer>();
        sphereMesh->mesh = Mesh::CreateSphere(1.0f, 32);
        sphereMesh->color = glm::vec3(
            0.3f + (i * 0.15f),
            0.5f,
            1.0f - (i * 0.15f)
        );
        auto* sphereTransform = sphere->GetComponent<Transform>();
        sphereTransform->position = glm::vec3(
            cos(angle) * radius,
            1.5f,
            sin(angle) * radius
        );
    }

    // Kup duvarlar (test icin)
    Entity* wall1 = scene.CreateEntity("Wall 1");
    auto* wall1Mesh = wall1->AddComponent<MeshRenderer>();
    wall1Mesh->mesh = Mesh::CreateCube(1.0f);
    wall1Mesh->color = glm::vec3(0.8f, 0.8f, 0.2f);
    auto* wall1Transform = wall1->GetComponent<Transform>();
    wall1Transform->position = glm::vec3(5.0f, 0.5f, 0.0f);
    wall1Transform->scale = glm::vec3(1.0f, 3.0f, 10.0f);

    Entity* wall2 = scene.CreateEntity("Wall 2");
    auto* wall2Mesh = wall2->AddComponent<MeshRenderer>();
    wall2Mesh->mesh = Mesh::CreateCube(1.0f);
    wall2Mesh->color = glm::vec3(0.2f, 0.8f, 0.8f);
    auto* wall2Transform = wall2->GetComponent<Transform>();
    wall2Transform->position = glm::vec3(-5.0f, 0.5f, 0.0f);
    wall2Transform->scale = glm::vec3(1.0f, 3.0f, 10.0f);

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
            editor.SetEnabled(!editor.IsEnabled());
            std::cout << "Editor: " << (editor.IsEnabled() ? "ON" : "OFF") << std::endl;
        }

        // FPS kontrolcusu guncellemesi
        fpsController.Update(input, deltaTime);

        // Silah sistemi guncellemesi (mermi olusturma ile)
        weaponSystem.Update(player, input, &scene, &camera, &projectileSystem, deltaTime);
        weaponSystem.UpdateRecoil(weapon, &camera, deltaTime);

        // Mermi sistemi guncellemesi
        projectileSystem.Update(&scene, deltaTime);

        // 3D sahne cizimi
        renderer->BeginFrame();
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

        // ESC tusu ile cikis
        if (input->IsKeyPressed(GLFW_KEY_ESCAPE)) {
            break;
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
