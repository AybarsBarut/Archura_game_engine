#include "Input.h"

namespace Archura {

Input::Input(GLFWwindow* window)
    : m_Window(window)
    , m_MousePosition(0.0f)
    , m_LastMousePosition(0.0f)
    , m_MouseDelta(0.0f)
    , m_ScrollDelta(0.0f)
    , m_FirstMouse(true)
    , m_CursorLocked(false)
{
    // Kaydirma geri cagrisini ayarla
    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, ScrollCallback);
}

void Input::Update() {
    // Fare pozisyonunu guncelle
    double xPos, yPos;
    glfwGetCursorPos(m_Window, &xPos, &yPos);
    
    glm::vec2 currentPos((float)xPos, (float)yPos);
    
    if (m_FirstMouse) {
        m_LastMousePosition = currentPos;
        m_FirstMouse = false;
    }
    
    if (m_CursorLocked) {
        m_MouseDelta = currentPos - m_LastMousePosition;
    } else {
        m_MouseDelta = glm::vec2(0.0f); // Imlec serbestken delta uretme (Kamera sapmasini onler)
    }

    m_LastMousePosition = currentPos;
    m_MousePosition = currentPos;
    
    // Kaydirma degisimini sifirla (her karede geri cagridan gelir)
    m_ScrollDelta = 0.0f;
}

void Input::EndFrame() {
    // Update key states for next frame
    for (int i = 32; i <= GLFW_KEY_LAST; ++i) {
        m_PreviousKeys[i] = glfwGetKey(m_Window, i) == GLFW_PRESS;
    }
}

bool Input::IsKeyPressed(int keycode) const {
    int state = glfwGetKey(m_Window, keycode);
    return state == GLFW_PRESS;
}

bool Input::IsKeyJustPressed(int keycode) const {
    int state = glfwGetKey(m_Window, keycode);
    bool pressed = (state == GLFW_PRESS);
    // Key is pressed now, but wasn't in previous frame
    return pressed && !m_PreviousKeys[keycode];
}

bool Input::IsKeyDown(int keycode) const {
    int state = glfwGetKey(m_Window, keycode);
    return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool Input::IsKeyReleased(int keycode) const {
    int state = glfwGetKey(m_Window, keycode);
    return state == GLFW_RELEASE;
}

bool Input::IsMouseButtonPressed(int button) const {
    int state = glfwGetMouseButton(m_Window, button);
    return state == GLFW_PRESS;
}

bool Input::IsMouseButtonDown(int button) const {
    int state = glfwGetMouseButton(m_Window, button);
    return state == GLFW_PRESS;
}

bool Input::IsMouseButtonReleased(int button) const {
    int state = glfwGetMouseButton(m_Window, button);
    return state == GLFW_RELEASE;
}

void Input::SetCursorMode(int mode) {
    glfwSetInputMode(m_Window, GLFW_CURSOR, mode);
    bool locked = (mode == GLFW_CURSOR_DISABLED);
    
    if (locked != m_CursorLocked) {
        m_CursorLocked = locked;
        if (m_CursorLocked) {
            m_FirstMouse = true; // Sifirla ki kamera ziplamasin
            
            // Mevcut pozisyonu last olarak set et
            double xPos, yPos;
            glfwGetCursorPos(m_Window, &xPos, &yPos);
            m_LastMousePosition = glm::vec2((float)xPos, (float)yPos);
        }
    }
}

void Input::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (input) {
        input->m_ScrollDelta = (float)yOffset;
    }
}

} // namespace Archura
