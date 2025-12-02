#include "FPSController.h"
#include "../input/Input.h"
#include <GLFW/glfw3.h>
#include <iostream>

namespace Archura {

FPSController::FPSController(Camera* camera)
    : m_Camera(camera)
{
}

void FPSController::Update(Input* input, float deltaTime) {
    HandleMovement(input, deltaTime);
    HandleMouseLook(input, deltaTime);
}

void FPSController::HandleMovement(Input* input, float deltaTime) {
    // Koşma kontrolü (Shift)
    m_IsRunning = input->IsKeyDown(GLFW_KEY_LEFT_SHIFT);
    float currentSpeed = m_IsRunning ? m_RunSpeed : m_WalkSpeed;

    // WASD hareketi
    if (input->IsKeyDown(GLFW_KEY_W)) {
        m_Camera->ProcessKeyboard(Camera::MovementDirection::Forward, deltaTime * currentSpeed / m_WalkSpeed);
    }
    if (input->IsKeyDown(GLFW_KEY_S)) {
        m_Camera->ProcessKeyboard(Camera::MovementDirection::Backward, deltaTime * currentSpeed / m_WalkSpeed);
    }
    if (input->IsKeyDown(GLFW_KEY_A)) {
        m_Camera->ProcessKeyboard(Camera::MovementDirection::Left, deltaTime * currentSpeed / m_WalkSpeed);
    }
    if (input->IsKeyDown(GLFW_KEY_D)) {
        m_Camera->ProcessKeyboard(Camera::MovementDirection::Right, deltaTime * currentSpeed / m_WalkSpeed);
    }

    // Q/E yukarı/aşağı (fly mode için)
    if (input->IsKeyDown(GLFW_KEY_E)) {
        m_Camera->ProcessKeyboard(Camera::MovementDirection::Up, deltaTime * currentSpeed / m_WalkSpeed);
    }
    if (input->IsKeyDown(GLFW_KEY_Q)) {
        m_Camera->ProcessKeyboard(Camera::MovementDirection::Down, deltaTime * currentSpeed / m_WalkSpeed);
    }

    // FOV kontrolü (mouse scroll)
    float scrollDelta = input->GetMouseScrollDelta();
    if (scrollDelta != 0.0f) {
        m_Camera->ProcessMouseScroll(scrollDelta);
    }
}

void FPSController::HandleMouseLook(Input* input, float deltaTime) {
    // Sağ tık basılıyken kamera döndür
    if (input->IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
        // Cursor'u kilitle
        if (!input->IsCursorLocked()) {
            input->SetCursorMode(GLFW_CURSOR_DISABLED);
        }

        glm::vec2 mouseDelta = input->GetMouseDelta();
        m_Camera->ProcessMouseMovement(mouseDelta.x * m_MouseSensitivity, 
                                      -mouseDelta.y * m_MouseSensitivity); // Y eksenini ters çevir
    }
    else {
        // Cursor'u serbest bırak
        if (input->IsCursorLocked()) {
            input->SetCursorMode(GLFW_CURSOR_NORMAL);
        }
    }
}

} // namespace Archura
