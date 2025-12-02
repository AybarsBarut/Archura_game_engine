#include "Window.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

namespace Archura {

static bool s_GLFWInitialized = false;

static void GLFWErrorCallback(int error, const char* description) {
    std::cerr << "[GLFW Error " << error << "]: " << description << std::endl;
}

Window::Window(const WindowProps& props)
    : m_Window(nullptr)
    , m_Width(props.width)
    , m_Height(props.height)
    , m_VSync(props.vsync)
    , m_DeltaTime(0.0f)
    , m_LastFrameTime(0.0f)
    , m_FPS(0.0f)
    , m_FPSTimer(0.0f)
    , m_FrameCount(0)
{
    Init(props);
}

Window::~Window() {
    Shutdown();
}

void Window::Init(const WindowProps& props) {
    m_Title = props.title;

    // GLFW'yi sadece bir kez initialize et
    if (!s_GLFWInitialized) {
        int success = glfwInit();
        if (!success) {
            std::cerr << "GLFW initialization failed!" << std::endl;
            return;
        }
        glfwSetErrorCallback(GLFWErrorCallback);
        s_GLFWInitialized = true;
    }

    // OpenGL 3.3 Core Profile - GTX 1050 için optimize
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // 4x MSAA - GTX 1050 performansı için ideal

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Window oluştur
    GLFWmonitor* monitor = props.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), monitor, nullptr);
    
    if (!m_Window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return;
    }

    // OpenGL context'i aktif et
    glfwMakeContextCurrent(m_Window);

    // GLAD ile OpenGL fonksiyonlarını yükle
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD!" << std::endl;
        return;
    }

    // OpenGL bilgilerini yazdır
    std::cout << "OpenGL Info:" << std::endl;
    std::cout << "  Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "  Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "  Version: " << glGetString(GL_VERSION) << std::endl;

    // VSync ayarla
    SetVSync(m_VSync);

    // OpenGL ayarları - GTX 1050 için optimize
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE); // MSAA aktif
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Viewport ayarla
    glViewport(0, 0, m_Width, m_Height);

    // Framebuffer resize callback
    glfwSetFramebufferSizeCallback(m_Window, [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    });

    m_LastFrameTime = (float)glfwGetTime();
}

void Window::Shutdown() {
    if (m_Window) {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
}

void Window::Update() {
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
    UpdateDeltaTime();
}

void Window::UpdateDeltaTime() {
    float currentTime = (float)glfwGetTime();
    m_DeltaTime = currentTime - m_LastFrameTime;
    m_LastFrameTime = currentTime;

    // FPS hesapla
    m_FrameCount++;
    m_FPSTimer += m_DeltaTime;
    
    if (m_FPSTimer >= 1.0f) {
        m_FPS = m_FrameCount / m_FPSTimer;
        m_FrameCount = 0;
        m_FPSTimer = 0.0f;
    }
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(m_Window);
}

void Window::SetVSync(bool enabled) {
    m_VSync = enabled;
    glfwSwapInterval(enabled ? 1 : 0);
}

} // namespace Archura
