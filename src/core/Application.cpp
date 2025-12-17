#include "core/Application.h"
#include "Engine.h"
#include "Window.h"
#include "ImGuiLayer.h"
#include "../game/PauseMenu.h"
#include "input/Input.h"
#include "rendering/Mesh.h"
#include "rendering/Renderer.h"
#include "rendering/Camera.h"
#include "rendering/Skybox.h"
#include "rendering/HUDRenderer.h"
#include "game/FPSController.h"
#include "game/RenderSystem.h"
#include "game/Weapon.h"
#include "game/Projectile.h"
#include "game/PhysicsSystem.h"
#include "game/PhysicsSystem.h"
#include "game/ScriptSystem.h"
#include "game/ParticleSystem.h"
#include "editor/Editor.h"
#include "game/DevConsole.h"
#include "core/AudioSystem.h"
#include "network/NetworkManager.h"
#include "ecs/Entity.h"
#include "ecs/Component.h"
#include <iostream>
#include <vector>
#include <cstdlib>

namespace Archura {

    Application* Application::s_Instance = nullptr;

    Application::Application() {
        s_Instance = this;
    }

    Application::~Application() {
        // Shutdown systems
    }

    bool Application::Init() {
        // 1. Motor / Pencere Başlatma
        Engine::EngineConfig config;
        // Durumu yansıtmak için başlık güncellendi
        config.windowTitle = "Archura FPS Engine - Build 0.2"; 
        config.windowWidth = 1920;
        config.windowHeight = 1080;
        config.vsync = false; 
        config.fullscreen = false;

        if (!Engine::Get().Init(config)) {
            std::cerr << "Failed to initialize engine!" << std::endl;
            return false;
        }

        m_Window = std::unique_ptr<Window>(Engine::Get().GetWindow());

        // 2. Ses
        AudioSystem::Get().Init();

        // 3. Ağ (Network)
        NetworkManager::Get().Init();

        // 4. Konsol
        DevConsole::Get().Init();

        // 5. Editör / ImGui
        m_ImGuiLayer = std::make_unique<ImGuiLayer>();
        m_ImGuiLayer->Init(m_Window.get());
        
        return true;
    }

    void Application::Run() {
        if (!Init()) return;

        // --- SAHNE KURULUMU (main.cpp'den taşındı) ---
        Scene scene("Demo Scene");
        Camera camera(glm::vec3(0.0f, 2.0f, 10.0f));
        FPSController fpsController(&camera);
        
        RenderSystem renderSystem(&camera);
        renderSystem.Init(&scene);

        Skybox skybox;
        skybox.Init();

        HUDRenderer hudRenderer;
        hudRenderer.Init();
        hudRenderer.SetScreenSize(1920.0f, 1080.0f);

        Editor editor;
        editor.Init(Engine::Get().GetWindow());
        
        // --- DÜZELTME 1: Oyun Modunda Başla (İmleç Kilitli, Editör Gizli) ---
        bool devModeActive = false;
        bool isPaused = false; // Pause Durumu
        
        // Editor'u her zaman aktif tutacagiz (ImGui Context'i sursun diye),
        // ama cizim yapip yapmayacagina asagida karar verecegiz.
        editor.SetEnabled(true); 

        // Oyuncu Kurulumu
        Entity* player = scene.CreateEntity("Player");
        auto* weapon = player->AddComponent<Weapon>();
        weapon->InitInventory();
        auto* playerHealth = player->AddComponent<Health>();
        playerHealth->max = 100.0f;
        playerHealth->current = 100.0f;
        player->AddComponent<BoxCollider>()->size = glm::vec3(0.8f, 1.8f, 0.8f);

        // Sistemler
        PhysicsSystem physicsSystem;
        physicsSystem.Init(&scene);
        ScriptSystem scriptSystem;
        scriptSystem.Init(&scene);
        ParticleSystem particleSystem;
        particleSystem.Init(&scene);
        ProjectileSystem projectileSystem;

        // Duraklatma Menüsünü (PauseMenu) Başlat
        PauseMenu pauseMenu;

        // --- HARİTA OLUŞTURMA (Optimize Edildi) ---
        // TODO: Bunu daha sonra InstancedRenderer'a taşı
        Entity* ground = scene.CreateEntity("Ground");
        auto* groundMesh = ground->AddComponent<MeshRenderer>();
        groundMesh->mesh = Mesh::CreatePlane(100.0f, 100.0f, 10.0f);
        groundMesh->color = glm::vec3(0.2f, 0.25f, 0.2f);
        
        // Zemin carpistiricisi (BoxCollider)
        // Yukseklik (Y) olarak ince ama algilanabilir bir deger verelim (1.0f)
        // Pozisyonu biraz asagi ( -0.5f ) kaydirarak zemin yuzeyini 0.0f'da tutabiliriz, 
        // ama simdilik merkezde kalsin, karakterin ayagi batarsa duzeltiriz.
        auto* groundCol = ground->AddComponent<BoxCollider>();
        groundCol->size = glm::vec3(100.0f, 1.0f, 100.0f);
        // Plane mesh genellikle Y=0'da olusur. Collider merkezi entity pozisyonundadir (0,0,0).
        // 1 birim kalinlik verirsek yuzey Y=0.5'te olur. 
        // Bunu engellemek icin transform.position.y = -0.5f yapabiliriz VEYA collider center offset ekleyebiliriz.
        // Component.h'da center offset var mi? Evet (glm::vec3 center).
        groundCol->center = glm::vec3(0.0f, -0.5f, 0.0f);

        // Walls
        float mapSize = 50.0f;
        float wallHeight = 5.0f;
        float wallThickness = 2.0f;
        glm::vec3 wallColor(0.5f, 0.5f, 0.55f);
        
        // Kütüphaneyi (Batching) etkinleştirmek için Duvarlar için Paylaşılan Mesh
        Mesh* sharedWallMesh = Mesh::CreateCube(1.0f);
        
        auto CreateWall = [&](glm::vec3 pos, glm::vec3 scale) {
             Entity* wall = scene.CreateEntity("Wall");
             auto* m = wall->AddComponent<MeshRenderer>();
             m->mesh = sharedWallMesh; 
             m->color = wallColor;
             auto* t = wall->GetComponent<Transform>();
             t->position = pos;
             t->scale = scale;
             wall->AddComponent<BoxCollider>()->size = glm::vec3(1.0f);
        };

        CreateWall(glm::vec3(0, wallHeight/2, -mapSize), glm::vec3(mapSize*2, wallHeight, wallThickness));
        CreateWall(glm::vec3(0, wallHeight/2, mapSize), glm::vec3(mapSize*2, wallHeight, wallThickness));
        CreateWall(glm::vec3(-mapSize, wallHeight/2, 0), glm::vec3(wallThickness, wallHeight, mapSize*2));
        CreateWall(glm::vec3(mapSize, wallHeight/2, 0), glm::vec3(wallThickness, wallHeight, mapSize*2));

        // --- OYUN DÖNGÜSÜ (GAME LOOP) ---
        auto* window = Engine::Get().GetWindow();
        auto* input = Engine::Get().GetInput();
        auto* renderer = Engine::Get().GetRenderer();

        // Başlangıçta imlecin kilitli olduğundan emin ol
        input->SetCursorMode(GLFW_CURSOR_DISABLED);

        while (!window->ShouldClose() && m_Running) {
            float time = (float)glfwGetTime();
            float deltaTime = time - (float)m_LastFrameTime;
            m_LastFrameTime = time;

            // Önce Olayları (Events) Dinle
            window->Update(); 
            input->Update();

            // --- GİRİŞ İŞLEME (Basitleştirilmiş) ---
            static bool escHandled = false;
            static bool tabHandled = false;
            
            // 1. ESC Tuşu: Duraklatmayı Aç/Kapa
            if (input->IsKeyPressed(GLFW_KEY_ESCAPE)) {
                 if (!escHandled) {
                     isPaused = !isPaused;
                     escHandled = true;
                     
                     // İmleç Durum Mantığı
                     if (isPaused) {
                         input->SetCursorMode(GLFW_CURSOR_NORMAL);
                     } else {
                         input->SetCursorMode(GLFW_CURSOR_DISABLED);
                     }
                 }
            } else {
                escHandled = false;
            }

            // 2. TAB Tuşu: Geliştirici Modunu Aç/Kapa
            if (input->IsKeyPressed(GLFW_KEY_TAB)) {
                if (!tabHandled) {
                    devModeActive = !devModeActive;
                    tabHandled = true;
                    
                    if (devModeActive) { 
                        input->SetCursorMode(GLFW_CURSOR_NORMAL);
                    } else {
                        // Oyun modu varsayılanlarına döner
                         if (!isPaused) input->SetCursorMode(GLFW_CURSOR_DISABLED);
                    }
                }
            } else {
                tabHandled = false;
            }

            // 3. Fare Tıklama Mantığı (Sadece Oyun Modunda)
            if (!isPaused && !devModeActive) {
                if (input->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
                    // Pencereye tıklanırsa imleci kilitle
                    input->SetCursorMode(GLFW_CURSOR_DISABLED);
                }
            }

            // Game Update Loop (Pause ise durdur)
            if (!isPaused) {
                fpsController.Update(input, &scene, deltaTime, &projectileSystem);
                physicsSystem.Update(deltaTime); 
                scriptSystem.Update(deltaTime);
                scriptSystem.Update(deltaTime);
                particleSystem.Update(deltaTime);
                AudioSystem::Get().Update(&scene, &camera);
            }

            // Network
            if (NetworkManager::Get().IsServer()) {
                NetworkManager::Get().UpdateServer();
            } else {
                NetworkManager::Get().UpdateClient();
            }

            // Çizim (Render)
            renderer->BeginFrame();
            
            float aspectRatio = (float)window->GetWidth() / (float)window->GetHeight();
            skybox.Draw(camera, aspectRatio);
            
            renderSystem.Update(deltaTime);
            
            // Geliştirici Modundaysa Hata Ayıklama Çarpıştırıcılarını Çiz
            if (devModeActive) {
                renderSystem.DrawColliders();
            }
            
            // HUD
            if (!devModeActive) {
                hudRenderer.BeginHUD((float)window->GetWidth(), (float)window->GetHeight());
                hudRenderer.DrawCrosshair();  
                hudRenderer.DrawHealthBar(playerHealth->current, playerHealth->max, 20, 20, 200, 20);
                hudRenderer.DrawAmmoCounter(weapon->stats.currentMag, weapon->stats.magSize, window->GetWidth() - 220, 20);
                hudRenderer.EndHUD();
            }

            // --- EDİTÖR & ARAYÜZ ÇİZİMİ ---
            m_ImGuiLayer->BeginFrame(); 
            
            if (devModeActive) {
                // Editor modu: Dockspace ve paneller
                editor.BeginDockSpace();
                editor.DrawEditorUI();
                editor.Update(&scene, deltaTime, (deltaTime > 0.0f) ? 1.0f / deltaTime : 0.0f);
            }
            
            if (isPaused) {
                // İmlecin görünür olduğundan emin ol
                input->SetCursorMode(GLFW_CURSOR_NORMAL);
                
                // Yeni Duraklatma Menüsünü Çiz
                pauseMenu.Render(isPaused, fpsController, *m_Window);
                
                 // Mantık kontrolü: Eğer menü devam etmeyi istediyse (isPaused false olduysa)
                if (!isPaused) {
                     input->SetCursorMode(GLFW_CURSOR_DISABLED);
                }
            }

            m_ImGuiLayer->EndFrame();
            
            // Swap Buffer window->Update() veya dahili döngü tarafından yönetilir

    }

}

} // namespace Archura
