#pragma once

#include <string>
#include <memory>

// Prevent GLFW from including Windows.h (to avoid APIENTRY conflicts)
#define GLFW_INCLUDE_NONE

// Forward declarations
struct GLFWwindow;

namespace Archura {

/**
 * @brief Window sınıfı - GLFW window yönetimi ve OpenGL context
 * 
 * GTX 1050 hedefli, yüksek performanslı window oluşturur
 */
class Window {
public:
    struct WindowProps {
        std::string title;
        unsigned int width;
        unsigned int height;
        bool vsync;
        bool fullscreen;

        WindowProps(
            const std::string& title = "Archura Engine",
            unsigned int width = 1920,
            unsigned int height = 1080,
            bool vsync = false,
            bool fullscreen = false
        ) : title(title), width(width), height(height), vsync(vsync), fullscreen(fullscreen) {}
    };

public:
    Window(const WindowProps& props = WindowProps());
    ~Window();

    // Copy constructor ve assignment operator'ı disable et
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void Update();
    bool ShouldClose() const;
    
    void SetVSync(bool enabled);
    bool IsVSync() const { return m_VSync; }

    void SetFullscreen(bool enabled);

    bool IsFullscreen() const { return m_Fullscreen; }

    void SetResolution(unsigned int width, unsigned int height); // New: Resolution support

    unsigned int GetWidth() const { return m_Width; }
    unsigned int GetHeight() const { return m_Height; }
    float GetAspectRatio() const { return (float)m_Width / (float)m_Height; }

    GLFWwindow* GetNativeWindow() const { return m_Window; }

    // FPS tracking
    // FPS tracking
    float GetDeltaTime() const { return m_DeltaTime; }
    float GetFPS() const { return m_FPS; }
    float GetLastFrameTime() const { return m_LastFrameTime; }

private:
    void Init(const WindowProps& props);
    void Shutdown();
    void UpdateDeltaTime();

private:
    GLFWwindow* m_Window;
    std::string m_Title;
    unsigned int m_Width, m_Height;
    bool m_VSync;
    bool m_Fullscreen;

    // Performans tracking
    float m_DeltaTime;
    float m_LastFrameTime;
    float m_FPS;
    float m_FPSTimer;
    int m_FrameCount;
};

} // namespace Archura
