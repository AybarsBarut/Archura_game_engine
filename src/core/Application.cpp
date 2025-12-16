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
#include "game/ScriptSystem.h"
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
        // 1. Engine / Window Init
        Engine::EngineConfig config;
        // Updated title to reflect state
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

        // 2. Audio
        AudioSystem::Get().Init();

        // 3. Network
        NetworkManager::Get().Init();

        // 4. Console
        DevConsole::Get().Init();

        // 5. Editor / ImGui
        m_ImGuiLayer = std::make_unique<ImGuiLayer>();
        m_ImGuiLayer->Init(m_Window.get());
        
        return true;
    }

    void Application::Run() {
        if (!Init()) return;

        // --- SCENE SETUP (Migrated from main.cpp) ---
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
        
        // --- FIX 1: Start in Game Mode (Cursor Locked, Editor Hidden) ---
        bool devModeActive = false;
        bool isPaused = false; // Pause Durumu
        
        // Editor'u her zaman aktif tutacagiz (ImGui Context'i sursun diye),
        // ama cizim yapip yapmayacagina asagida karar verecegiz.
        editor.SetEnabled(true); 

        // Player Setup
        Entity* player = scene.CreateEntity("Player");
        auto* weapon = player->AddComponent<Weapon>();
        weapon->InitInventory();
        auto* playerHealth = player->AddComponent<Health>();
        playerHealth->max = 100.0f;
        playerHealth->current = 100.0f;
        player->AddComponent<BoxCollider>()->size = glm::vec3(0.8f, 1.8f, 0.8f);

        // Systems
        PhysicsSystem physicsSystem;
        physicsSystem.Init(&scene);
        ScriptSystem scriptSystem;
        scriptSystem.Init(&scene);
        ProjectileSystem projectileSystem;

        // Initialize PauseMenu
        PauseMenu pauseMenu;

        // --- MAP GENERATION (Optimized) ---
        // TODO: Move this to InstancedRenderer later
        Entity* ground = scene.CreateEntity("Ground");
        auto* groundMesh = ground->AddComponent<MeshRenderer>();
        groundMesh->mesh = Mesh::CreatePlane(100.0f, 100.0f, 10.0f);
        groundMesh->color = glm::vec3(0.2f, 0.25f, 0.2f);

        // Walls
        float mapSize = 50.0f;
        float wallHeight = 5.0f;
        float wallThickness = 2.0f;
        glm::vec3 wallColor(0.5f, 0.5f, 0.55f);
        
        // Shared Mesh for Walls to enable Batching
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

        // --- GAME LOOP ---
        auto* window = Engine::Get().GetWindow();
        auto* input = Engine::Get().GetInput();
        auto* renderer = Engine::Get().GetRenderer();

        // Ensure cursor is locked at start
        input->SetCursorMode(GLFW_CURSOR_DISABLED);

        while (!window->ShouldClose() && m_Running) {
            float time = (float)glfwGetTime();
            float deltaTime = time - (float)m_LastFrameTime;
            m_LastFrameTime = time;

            // Poll Events FIRST
            window->Update(); 
            input->Update();

            // --- INPUT HANDLING (Simplified) ---
            static bool escHandled = false;
            static bool tabHandled = false;
            
            // 1. ESC Key: Toggle Pause
            if (input->IsKeyPressed(GLFW_KEY_ESCAPE)) {
                 if (!escHandled) {
                     isPaused = !isPaused;
                     escHandled = true;
                     
                     // Cursor State Logic
                     if (isPaused) {
                         input->SetCursorMode(GLFW_CURSOR_NORMAL);
                     } else {
                         input->SetCursorMode(GLFW_CURSOR_DISABLED);
                     }
                 }
            } else {
                escHandled = false;
            }

            // 2. TAB Key: Toggle Dev Mode
            if (input->IsKeyPressed(GLFW_KEY_TAB)) {
                if (!tabHandled) {
                    devModeActive = !devModeActive;
                    tabHandled = true;
                    
                    if (devModeActive) { 
                        input->SetCursorMode(GLFW_CURSOR_NORMAL);
                    } else {
                        // Returns to game mode defaults
                         if (!isPaused) input->SetCursorMode(GLFW_CURSOR_DISABLED);
                    }
                }
            } else {
                tabHandled = false;
            }

            // 3. Mouse Clicking Logic (Only in Game Mode)
            if (!isPaused && !devModeActive) {
                if (input->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) {
                    // Lock cursor if clicked in window
                    input->SetCursorMode(GLFW_CURSOR_DISABLED);
                }
            }

            // Game Update Loop (Pause ise durdur)
            if (!isPaused) {
                fpsController.Update(input, &scene, deltaTime);
                physicsSystem.Update(deltaTime); 
                scriptSystem.Update(deltaTime);
            }

            // Network
            if (NetworkManager::Get().IsServer()) {
                NetworkManager::Get().UpdateServer();
            } else {
                NetworkManager::Get().UpdateClient();
            }

            // Render
            renderer->BeginFrame();
            
            float aspectRatio = (float)window->GetWidth() / (float)window->GetHeight();
            skybox.Draw(camera, aspectRatio);
            
            renderSystem.Update(deltaTime);
            
            // Draw Debug Colliders if in Dev Mode
            if (devModeActive) {
                renderSystem.DrawColliders();
            }
            
            // HUD
            if (!devModeActive) {
                hudRenderer.BeginHUD();
                hudRenderer.DrawCrosshair(); 
                hudRenderer.DrawHealthBar(playerHealth->current, playerHealth->max, 20, 20, 200, 20);
                hudRenderer.DrawAmmoCounter(weapon->stats.currentMag, weapon->stats.magSize, window->GetWidth() - 220, 20);
                hudRenderer.EndHUD();
            }

            // --- EDITOR & GUI RENDER ---
            m_ImGuiLayer->BeginFrame(); 
            
            if (devModeActive) {
                // Editor modu: Dockspace ve paneller
                editor.BeginDockSpace();
                editor.DrawEditorUI();
                editor.Update(&scene, deltaTime, (deltaTime > 0.0f) ? 1.0f / deltaTime : 0.0f);
            }
            
            if (isPaused) {
                // Ensure cursor is visible
                input->SetCursorMode(GLFW_CURSOR_NORMAL);
                
                // Draw new Pause Menu
                pauseMenu.Render(isPaused, fpsController, *m_Window);
                
                 // Logic check: If menu requested resume (isPaused became false)
                if (!isPaused) {
                     input->SetCursorMode(GLFW_CURSOR_DISABLED);
                }
            }

            m_ImGuiLayer->EndFrame();
            
            // Swap Buffer handled by window->Update() or internal loop

    }

}

} // namespace Archura
