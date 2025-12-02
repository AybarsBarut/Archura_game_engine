#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace Archura {

/**
 * @brief Input sistemi - Keyboard ve mouse yönetimi
 * 
 * Polling-based input system, FPS kontrolü için optimize edilmiş
 */
class Input {
public:
    Input(GLFWwindow* window);
    ~Input() = default;

    void Update();

    // Keyboard
    bool IsKeyPressed(int keycode) const;
    bool IsKeyDown(int keycode) const;
    bool IsKeyReleased(int keycode) const;

    // Mouse
    bool IsMouseButtonPressed(int button) const;
    bool IsMouseButtonDown(int button) const;
    bool IsMouseButtonReleased(int button) const;

    glm::vec2 GetMousePosition() const { return m_MousePosition; }
    glm::vec2 GetMouseDelta() const { return m_MouseDelta; }
    float GetMouseScrollDelta() const { return m_ScrollDelta; }

    // Mouse için
    void SetCursorMode(int mode); // GLFW_CURSOR_NORMAL, GLFW_CURSOR_HIDDEN, GLFW_CURSOR_DISABLED
    bool IsCursorLocked() const { return m_CursorLocked; }

private:
    static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);

private:
    GLFWwindow* m_Window;
    
    // Mouse state
    glm::vec2 m_MousePosition;
    glm::vec2 m_LastMousePosition;
    glm::vec2 m_MouseDelta;
    float m_ScrollDelta;
    bool m_FirstMouse;
    bool m_CursorLocked;
};

} // namespace Archura
