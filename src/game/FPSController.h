#pragma once

#include "../rendering/Camera.h"

namespace Archura {

class Input;

/**
 * @brief FPS Controller - First-person karakter kontrolü
 * 
 * WASD hareket, mouse look, koşma, zıplama
 */
struct KeyBindings {
    int forward;
    int backward;
    int left;
    int right;
    int jump;
    int sprint;
};

class FPSController {
public:
    FPSController(Camera* camera);
    ~FPSController() = default;

    void Update(Input* input, class Scene* scene, float deltaTime);

    // Ayarlar
    void SetWalkSpeed(float speed) { m_WalkSpeed = speed; }
    void SetRunSpeed(float speed) { m_RunSpeed = speed; }
    void SetJumpHeight(float height) { m_JumpHeight = height; }
    void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }
    void SetKeyBindings(const KeyBindings& bindings) { m_Bindings = bindings; }

    float GetWalkSpeed() const { return m_WalkSpeed; }
    float GetRunSpeed() const { return m_RunSpeed; }
    float GetJumpHeight() const { return m_JumpHeight; }
    float GetMouseSensitivity() const { return m_MouseSensitivity; }
    KeyBindings& GetKeyBindings() { return m_Bindings; }

    // State
    bool IsGrounded() const { return m_IsGrounded; }
    bool IsRunning() const { return m_IsRunning; }
    
    void SetGravityEnabled(bool enabled) { m_GravityEnabled = enabled; }
    bool IsGravityEnabled() const { return m_GravityEnabled; }

private:
    void HandleMovement(Input* input, class Scene* scene, float deltaTime);
    void HandleMouseLook(Input* input, float deltaTime);
    bool CheckCollision(const glm::vec3& position, class Scene* scene, float* outGroundHeight = nullptr, float stepHeight = 0.0f);

private:
    Camera* m_Camera;

    // Movement settings
    float m_WalkSpeed = 5.0f;
    float m_RunSpeed = 10.0f;
    float m_JumpHeight = 2.0f;
    float m_MouseSensitivity = 0.1f;
    float m_Gravity = -9.81f;
    bool m_GravityEnabled = true; // Yercekimi varsayilan olarak acik
    KeyBindings m_Bindings;

    // State
    bool m_IsGrounded = true;
    bool m_IsRunning = false;
    float m_VerticalVelocity = 0.0f;
};

} // namespace Archura
