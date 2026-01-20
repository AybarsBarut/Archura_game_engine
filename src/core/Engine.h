#pragma once

#include <memory>
#include <string>

namespace Archura {

// Forward declarations
class Window;
class Renderer;
class Input;

/**
 * @brief Ana Engine sınıfı - Motor lifecycle yönetimi
 * 
 * Singleton pattern ile tüm sistemleri koordine eder
 */
class Engine {
public:
    struct EngineConfig {
        std::string windowTitle = "Archura Engine";
        unsigned int windowWidth = 1920;
        unsigned int windowHeight = 1080;
        bool vsync = false;
        bool fullscreen = false;
        bool editorMode = true; // Editor UI'ı aktif mi?
    };

public:
    static Engine& Get();

    bool Init(const EngineConfig& config = EngineConfig());
    void Run();
    void Shutdown();

    // Getter'lar
    Window* GetWindow() const { return m_Window.get(); }
    Renderer* GetRenderer() const { return m_Renderer.get(); }
    Input* GetInput() const { return m_Input.get(); }

    bool IsRunning() const { return m_Running; }
    bool IsEditorMode() const { return m_EditorMode; }

private:
    Engine() = default;
    ~Engine() = default;

    // Singleton - copy ve move disable
    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;

    void Update(float deltaTime);
    void Render();

private:
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Renderer> m_Renderer;
    std::unique_ptr<Input> m_Input;

    bool m_Running = false;
    bool m_EditorMode = true;
};

} // namespace Archura
