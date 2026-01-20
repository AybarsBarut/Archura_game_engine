#include "core/Application.h"
#include "core/AudioSystem.h"
#include "core/Engine.h"
#include "core/ImGuiLayer.h"
#include "core/Window.h"

#include "ecs/Component.h"
#include "ecs/Entity.h"

#include "editor/Editor.h"

#include "game/CommandRegistry.h"
#include "game/DevConsole.h"
#include "game/FPSController.h"
#include "game/ParticleSystem.h"
#include "game/PauseMenu.h"
#include "game/PhysicsSystem.h"
#include "game/Projectile.h"
#include "game/ProjectileSystem.h"
#include "game/RenderSystem.h"
#include "game/ScriptSystem.h"
#include "game/Weapon.h"

#include "input/Input.h"
#include "network/NetworkManager.h"

#include "rendering/Camera.h"
#include "rendering/HUDRenderer.h"
#include "rendering/Mesh.h"
#include "rendering/Renderer.h"
#include "rendering/Skybox.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

namespace Archura {

Application* Application::s_Instance = nullptr;

Application::Application() {
    s_Instance = this;
}

Application::~Application() {
}

void Application::SetSensitivity(float sens) {
    if (m_FPSController) {
        m_FPSController->SetMouseSensitivity(sens);
        std::cout << "Sensitivity set to " << sens << std::endl;
    }
}

void Application::SetDevMode(bool enabled) {
    m_DevModeActive = enabled;
}

bool Application::Init() {
    Engine::EngineConfig config;
    config.windowTitle = "Archura FPS Engine - Build 0.2";
    config.windowWidth = 1920;
    config.windowHeight = 1080;
    config.vsync = false;
    config.fullscreen = false;

    if (!Engine::Get().Init(config)) {
        std::cerr << "Failed to initialize engine!" << std::endl;
        return false;
    }

    // ❗ Window Engine tarafından sahipleniliyor
    m_Window = Engine::Get().GetWindow();

    AudioSystem::Get().Init();
    NetworkManager::Get().Init();
    DevConsole::Get().Init();

    // Register Commands
    CommandRegistry::Get().RegisterCommand(
        "fps_limit", [](const std::vector<std::string>& args) {
            if (!args.empty()) {
                float limit = std::stof(args[0]);
                Application::Get().SetFPSLimit(limit);
            }
        });

    CommandRegistry::Get().RegisterCommand(
        "sensitivity", [](const std::vector<std::string>& args) {
            if (!args.empty()) {
                Application::Get().SetSensitivity(std::stof(args[0]));
            }
        });

    CommandRegistry::Get().RegisterCommand(
        "debug_mode", [](const std::vector<std::string>& args) {
             if (!args.empty()) {
                // Try catch block for stoi safety could be added
                try {
                    int mode = std::stoi(args[0]);
                    Application::Get().SetDevMode(mode != 0);
                    std::cout << "Debug Mode set to: " << (mode != 0 ? "ON" : "OFF") << std::endl;
                } catch (...) {
                    std::cout << "Invalid argument for debug_mode" << std::endl;
                }
             } else {
                 bool current = Application::Get().IsDevMode();
                 Application::Get().SetDevMode(!current);
                 std::cout << "Debug Mode toggled to: " << (!current ? "ON" : "OFF") << std::endl;
             }
        });

    CommandRegistry::Get().RegisterCommand(
        "bind", [](const std::vector<std::string>& args) {
            if (args.size() < 2) {
                DevConsole::Get().Log("Usage: bind <key_id> <command>");
                return;
            }
            try {
                int key = std::stoi(args[0]);
                std::string command = args[1];
                for (size_t i = 2; i < args.size(); ++i)
                    command += " " + args[i];

                DevConsole::Get().Bind(key, command);
            } catch (...) {
                DevConsole::Get().Log("Invalid key ID");
            }
        });

    CommandRegistry::Get().RegisterCommand(
        "gamma", [](const std::vector<std::string>& args) {
            if (args.empty()) return;
            try {
                float val = std::stof(args[0]);
                GLFWmonitor* monitor = glfwGetPrimaryMonitor();
                if (monitor) {
                    glfwSetGamma(monitor, val);
                }
            } catch (...) {}
        });

    CommandRegistry::Get().RegisterCommand(
        "brightness", [](const std::vector<std::string>& args) {
            if (!args.empty())
                CommandRegistry::Get().Dispatch("gamma " + args[0]);
        });

    CommandRegistry::Get().RegisterCommand(
        "windowed", [](const std::vector<std::string>&) {
             // Access Window via Application instance if needed, or Engine
             auto& window = Application::Get().GetWindow();
             window.SetFullscreen(false);
        });

    CommandRegistry::Get().RegisterCommand(
        "fullscreen", [](const std::vector<std::string>&) {
             auto& window = Application::Get().GetWindow();
             window.SetFullscreen(true);
        });

    CommandRegistry::Get().RegisterCommand(
        "quit", [](const std::vector<std::string>&) {
            Application::Get().Quit();
        });

    m_ImGuiLayer = std::make_unique<ImGuiLayer>();
    m_ImGuiLayer->Init(m_Window);

    return true;
}

void Application::Run() {
    if (!Init())
        return;

    Scene scene("Demo Scene");
    Camera camera(glm::vec3(0.0f, 5.0f, 10.0f));

    m_FPSController = std::make_unique<FPSController>(&camera);

    RenderSystem renderSystem(&camera);
    renderSystem.Init(&scene);

    Skybox skybox;
    skybox.Init();

    HUDRenderer hudRenderer;
    hudRenderer.Init();

    Editor editor;
    editor.Init(m_Window); 
    editor.SetEnabled(true);

    Entity* player = scene.CreateEntity("Player");
    auto* weapon = player->AddComponent<Weapon>();
    weapon->InitInventory();

    auto* playerHealth = player->AddComponent<Health>();
    playerHealth->max = 100.0f;
    playerHealth->current = 100.0f;

    // Physics is handled by FPSController mostly
    // auto* collider = player->AddComponent<BoxCollider>();
    // collider->size = glm::vec3(0.8f, 1.8f, 0.8f);

    PhysicsSystem physicsSystem;
    physicsSystem.Init(&scene);

    ScriptSystem scriptSystem;
    scriptSystem.Init(&scene);

    ParticleSystem particleSystem;
    particleSystem.Init(&scene);

    // Initialized ProjectileSystem
    ProjectileSystem projectileSystem;
    projectileSystem.Init(&scene);

    // --- SETUP ROBUST MAP (V2) ---
    // 1. Sun
    Entity* light = scene.CreateEntity("Sun");
    auto* lightComp = light->AddComponent<LightComponent>();
    lightComp->type = LightComponent::Type::Directional;
    lightComp->color = glm::vec3(1.0f, 0.98f, 0.9f);
    lightComp->intensity = 2.0f; // Varsayilan 2.0
    light->GetComponent<Transform>()->rotation = glm::vec3(-50.0f, -30.0f, 0.0f);
    light->GetComponent<Transform>()->position = glm::vec3(0.0f, 50.0f, 0.0f); // Golge haritasi icin gerekli konum

    // 1.5 Ambient Light (Fill)
    Entity* ambientLight = scene.CreateEntity("AmbientLight");
    auto* ambientComp = ambientLight->AddComponent<LightComponent>();
    ambientComp->type = LightComponent::Type::Circle; // Ambient/Fill Light
    ambientComp->color = glm::vec3(0.6f, 0.6f, 0.7f); // Hafif mavimsi golge 
    ambientComp->intensity = 0.5f;

    // 2. Floor
    Entity* floor = scene.CreateEntity("Floor");
    {
        auto* mesh = floor->AddComponent<MeshRenderer>();
        mesh->mesh = Mesh::CreateCube();
        mesh->color = glm::vec3(0.4f, 0.4f, 0.45f);

        auto* trans = floor->GetComponent<Transform>();
        trans->position = glm::vec3(0.0f, -1.0f, 0.0f);
        trans->scale = glm::vec3(100.0f, 1.0f, 100.0f);

        auto* col = floor->AddComponent<BoxCollider>();
        col->size = glm::vec3(1.0f, 1.0f, 1.0f);
        col->isTrigger = false;
        col->center = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // 3. Walls
    float mapSize = 100.0f;
    float wallHeight = 15.0f;
    float wallThick = 2.0f;
    float offset = mapSize * 0.5f;
    float wallY = (wallHeight * 0.5f) - 1.0f;

    struct WallDef { std::string name; glm::vec3 p; glm::vec3 s; glm::vec3 c; };
    std::vector<WallDef> walls = {
        { "Wall_North", {0, wallY, -offset}, {mapSize, wallHeight, wallThick}, {0.8f, 0.4f, 0.4f} },
        { "Wall_South", {0, wallY, offset},  {mapSize, wallHeight, wallThick}, {0.4f, 0.8f, 0.4f} },
        { "Wall_East",  {offset, wallY, 0},  {wallThick, wallHeight, mapSize}, {0.4f, 0.4f, 0.8f} },
        { "Wall_West",  {-offset, wallY, 0}, {wallThick, wallHeight, mapSize}, {0.8f, 0.8f, 0.4f} }
    };

    for(const auto& w : walls) {
        Entity* wall = scene.CreateEntity(w.name);
        auto* mesh = wall->AddComponent<MeshRenderer>();
        mesh->mesh = Mesh::CreateCube();
        mesh->color = w.c;
        wall->GetComponent<Transform>()->position = w.p;
        wall->GetComponent<Transform>()->scale = w.s;
        
        auto* col = wall->AddComponent<BoxCollider>();
        col->size = glm::vec3(1.0f, 1.0f, 1.0f);
        col->isTrigger = false;
    }
    // ---------------------------

    PauseMenu pauseMenu;

    auto* window = Engine::Get().GetWindow();
    auto* input = Engine::Get().GetInput();
    auto* renderer = Engine::Get().GetRenderer();

    input->SetCursorMode(GLFW_CURSOR_DISABLED);

    while (!window->ShouldClose() && m_Running) {
        float time = (float)glfwGetTime();
        float deltaTime = time - (float)m_LastFrameTime;
        m_LastFrameTime = time;

        // 1. Poll Events
        window->Update(); 
        input->Update(); // Calculate Mouse Delta here

        // Toggle Dev Mode with TAB
        if (input->IsKeyJustPressed(258)) { // GLFW_KEY_TAB
             SetDevMode(!m_DevModeActive);
             std::cout << "[INFO] DevMode toggled: " << (m_DevModeActive ? "ON" : "OFF") << std::endl;
        }

        // Toggle Pause with ESC (Debounced)
        if (input->IsKeyJustPressed(256)) { // GLFW_KEY_ESCAPE
            m_IsPaused = !m_IsPaused;
            input->SetCursorMode(m_IsPaused ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        }


        // Toggle Console with grave accent (~)
        if (input->IsKeyJustPressed(96)) { // GLFW_KEY_GRAVE_ACCENT
            DevConsole::Get().Toggle();
        }

        // 2. Game Logic
        if (!m_IsPaused) {
            // FPSController Update
            m_FPSController->Update(input, &scene, deltaTime, &projectileSystem);
            
            projectileSystem.Update(deltaTime);
            physicsSystem.Update(deltaTime);
            scriptSystem.Update(deltaTime);
            particleSystem.Update(deltaTime);
            AudioSystem::Get().Update(&scene, &camera);
        }

        // 3. Rendering
        renderer->BeginFrame(); // Clear Screen
        m_ImGuiLayer->BeginFrame(); // Starts ImGui Frame

        // Render 3D Scene
        renderSystem.Update(deltaTime);
        
        // Render UI (ImGui)
        if (m_DevModeActive) {
            editor.BeginDockSpace();
            editor.DrawMenuBar(&scene);
            editor.DrawEditorUI(&scene);
            editor.DrawOverlay(&scene, &camera);
        }
        
        DevConsole::Get().Render();
        
        // Pass dependencies to PauseMenu
        pauseMenu.Render(m_IsPaused, *m_FPSController, *window);

        m_ImGuiLayer->EndFrame(); // Render ImGui Draw Data
        renderer->EndFrame(); // Finalize Frame
        
        // Update Input State for next frame (PreviousKeys = CurrentKeys)
        input->EndFrame();
    }
}

} // namespace Archura
