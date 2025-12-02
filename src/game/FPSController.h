#pragma once

#include "../rendering/Camera.h"

namespace Archura {

class Input;

/**
 * @brief FPS Controller - First-person karakter kontrolü
 * 
 * WASD hareket, mouse look, koşma, zıplama
 */
class FPSController {
public:
    FPSController(Camera* camera);
    ~FPSController() = default;

    void Update(Input* input, float deltaTime);

    // Ayarlar
    void SetWalkSpeed(float speed) { m_WalkSpeed = speed; }
    void SetRunSpeed(float speed) { m_RunSpeed = speed; }
    void SetJumpHeight(float height) { m_JumpHeight = height; }
    void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }

    float GetWalkSpeed() const { return m_WalkSpeed; }
    float GetRunSpeed() const { return m_RunSpeed; }
    float GetJumpHeight() const { return m_JumpHeight; }
    float GetMouseSensitivity() const { return m_MouseSensitivity; }

    // State
    bool IsGrounded() const { return m_IsGrounded; }
    bool IsRunning() const { return m_IsRunning; }

private:
    void HandleMovement(Input* input, float deltaTime);
    void HandleMouseLook(Input* input, float deltaTime);

private:
    Camera* m_Camera;

    // Movement settings
    float m_WalkSpeed = 5.0f;
    float m_RunSpeed = 10.0f;
    float m_JumpHeight = 2.0f;
    float m_MouseSensitivity = 0.1f;
    float m_Gravity = -9.81f;

    // State
    bool m_IsGrounded = true;
    bool m_IsRunning = false;
    float m_VerticalVelocity = 0.0f;
};

} // namespace Archura
