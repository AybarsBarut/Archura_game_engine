#include "Engine.h"
#include "Window.h"
#include "../rendering/Renderer.h"
#include "../input/Input.h"
#include <iostream>

namespace Archura {

Engine& Engine::Get() {
    static Engine instance;
    return instance;
}

bool Engine::Init(const EngineConfig& config) {
    std::cout << "=== Archura Engine Initialization ===" << std::endl;
    std::cout << "Target: GTX 1050 @ 120+ FPS" << std::endl;
    std::cout << "Graphics API: OpenGL 3.3+" << std::endl;
    std::cout << "======================================" << std::endl;

    m_EditorMode = config.editorMode;

    // Pencere olustur
    Window::WindowProps windowProps(
        config.windowTitle,
        config.windowWidth,
        config.windowHeight,
        config.vsync,
        config.fullscreen
    );
    
    m_Window = std::make_unique<Window>(windowProps);
    if (!m_Window->GetNativeWindow()) {
        std::cerr << "Failed to create window!" << std::endl;
        return false;
    }

    // Giris sistemi
    m_Input = std::make_unique<Input>(m_Window->GetNativeWindow());

    // Goruntu sistemi
    m_Renderer = std::make_unique<Renderer>();
    if (!m_Renderer->Init()) {
        std::cerr << "Failed to initialize renderer!" << std::endl;
        return false;
    }

    m_Running = true;
    std::cout << "Engine initialized successfully!" << std::endl;
    return true;
}

void Engine::Run() {
    std::cout << "Engine is running..." << std::endl;

    // Ana oyun dongusu
    while (m_Running && !m_Window->ShouldClose()) {
        float deltaTime = m_Window->GetDeltaTime();
        
        // Update
        m_Input->Update();
        Update(deltaTime);

        // Render
        Render();

        // Window update
        m_Window->Update();

        // ESC tusu ile cikis
        if (m_Input->IsKeyPressed(GLFW_KEY_ESCAPE)) {
            m_Running = false;
        }
    }

    std::cout << "Engine shutting down..." << std::endl;
}

void Engine::Shutdown() {
    if (m_Renderer) {
        m_Renderer->Shutdown();
    }
    
    m_Input.reset();
    m_Renderer.reset();
    m_Window.reset();
    
    m_Running = false;
    std::cout << "Engine shutdown complete." << std::endl;
}

void Engine::Update(float deltaTime) {
    // Her karede FPS bilgisini goster (konsolda spam yapmamak icin 1 saniyede bir)
    static float fpsTimer = 0.0f;
    fpsTimer += deltaTime;
    
    if (fpsTimer >= 1.0f) {
        std::cout << "FPS: " << m_Window->GetFPS() 
                  << " | Frame Time: " << (deltaTime * 1000.0f) << "ms" << std::endl;
        fpsTimer = 0.0f;
    }

    // Burada oyun mantigi guncellemeleri yapilacak
    // - ECS update
    // - Physics update
    // - Animation update
    // vs...
}

void Engine::Render() {
    m_Renderer->BeginFrame();
    
    // Burada goruntu islemleri yapilacak
    // - Scene rendering
    // - Editor UI (ImGui)
    // vs...
    
    m_Renderer->EndFrame();
}

} // namespace Archura
