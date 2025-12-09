#include "core/Application.h"
#include "Engine.h"
#include "Window.h"
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
        // Editor init moved to Run() to keep local scope or we can move it to member if needed later
        
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
        editor.SetEnabled(false);

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

             // --- FIX 2: TAB Key Logic (Instant Switch) ---
             if (input->IsKeyPressed(GLFW_KEY_TAB)) {
                 devModeActive = !devModeActive;
                 editor.SetEnabled(devModeActive);
                 
                 if (devModeActive) { // Switched TO Dev Mode
                     input->SetCursorMode(GLFW_CURSOR_NORMAL);
                     std::cout << "Switched to Developer Mode" << std::endl;
                 } else { // Switched TO Game Mode
                     input->SetCursorMode(GLFW_CURSOR_DISABLED);
                     std::cout << "Switched to Game Mode" << std::endl;
                 }
             }

             // --- FIX 3: ESC Logic (Prevent Crash/Immediate Exit) ---
             if (input->IsKeyPressed(GLFW_KEY_ESCAPE)) {
                 if (input->IsCursorLocked()) {
                     // Game Mode -> Pause/Menu Mode (Unlock Cursor)
                     input->SetCursorMode(GLFW_CURSOR_NORMAL);
                 } else {
                     // Cursor Unlocked -> Back to Game (Lock) only if NO dev mode
                     if (!devModeActive) {
                         input->SetCursorMode(GLFW_CURSOR_DISABLED);
                     }
                     // If Dev Mode is Active, ESC might do nothing or close a window, 
                     // but explicitly deciding NOT to quit app here. 
                 }
            }

            // Game Mode Fallback: Click to capture cursor
            if (!devModeActive && input->IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT) && !input->IsCursorLocked()) {
                // Check if we are hovering over an ImGui window? 
                // Since editor is disabled, we are not. Capture cursor.
                if (!editor.IsEnabled()) {
                     input->SetCursorMode(GLFW_CURSOR_DISABLED);
                }
            }

            fpsController.Update(input, &scene, deltaTime);
            physicsSystem.Update(deltaTime);
            scriptSystem.Update(deltaTime);

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
            
            // HUD
            if (!devModeActive) {
                hudRenderer.BeginHUD();
                hudRenderer.DrawCrosshair();
                hudRenderer.DrawHealthBar(playerHealth->current, playerHealth->max, 20, 20, 200, 20);
                hudRenderer.DrawAmmoCounter(weapon->stats.currentMag, weapon->stats.magSize, window->GetWidth() - 220, 20);
                hudRenderer.EndHUD();
            }

            // --- FIX 4: Call Editor Update ---
            if (devModeActive) {
                editor.BeginFrame(); 
                // Ensure deltaTime is not zero
                editor.Update(&scene, deltaTime, (deltaTime > 0.0f) ? 1.0f / deltaTime : 0.0f);
                editor.EndFrame(); 
            }

            // Swap Buffer handled by window->Update() or internal loop
        }
    }

}
