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
    // Scroll callback ayarla
    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, ScrollCallback);
}

void Input::Update() {
    // Mouse pozisyonunu güncelle
    double xPos, yPos;
    glfwGetCursorPos(m_Window, &xPos, &yPos);
    
    glm::vec2 currentPos((float)xPos, (float)yPos);
    
    if (m_FirstMouse) {
        m_LastMousePosition = currentPos;
        m_FirstMouse = false;
    }
    
    m_MouseDelta = currentPos - m_LastMousePosition;
    m_LastMousePosition = currentPos;
    m_MousePosition = currentPos;
    
    // Scroll delta'yı sıfırla (her frame'de callback'ten gelir)
    m_ScrollDelta = 0.0f;
}

bool Input::IsKeyPressed(int keycode) const {
    int state = glfwGetKey(m_Window, keycode);
    return state == GLFW_PRESS;
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
    m_CursorLocked = (mode == GLFW_CURSOR_DISABLED);
    
    if (m_CursorLocked) {
        m_FirstMouse = true; // Delta jump'ı önle
    }
}

void Input::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset) {
    Input* input = static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (input) {
        input->m_ScrollDelta = (float)yOffset;
    }
}

} // namespace Archura
