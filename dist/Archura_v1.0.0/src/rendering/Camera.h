#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Archura {

/**
 * @brief FPS Camera sınıfı - First-person perspective kamera
 * 
 * WASD hareket, mouse look kontrolü
 */
class Camera {
public:
    enum class MovementDirection {
        Forward,
        Backward,
        Left,
        Right,
        Up,
        Down
    };

public:
    Camera(const glm::vec3& position = glm::vec3(0.0f, 0.0f, 3.0f),
           const glm::vec3& up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f,
           float pitch = 0.0f);

    // View & Projection matrices
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix(float aspectRatio) const;

    // Kamera kontrolü
    void ProcessKeyboard(MovementDirection direction, float deltaTime);
    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yOffset);

    // Getters
    const glm::vec3& GetPosition() const { return m_Position; }
    const glm::vec3& GetFront() const { return m_Front; }
    const glm::vec3& GetUp() const { return m_Up; }
    const glm::vec3& GetRight() const { return m_Right; }
    
    float GetYaw() const { return m_Yaw; }
    float GetPitch() const { return m_Pitch; }
    float GetFOV() const { return m_FOV; }
    float GetNearPlane() const { return m_NearPlane; }
    float GetFarPlane() const { return m_FarPlane; }

    // Setters
    void SetPosition(const glm::vec3& position) { m_Position = position; }
    void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }
    void SetMouseSensitivity(float sensitivity) { m_MouseSensitivity = sensitivity; }
    void SetFOV(float fov) { m_FOV = fov; }

private:
    void UpdateCameraVectors();

private:
    // Kamera pozisyonu ve yönü
    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    // Euler angles
    float m_Yaw;
    float m_Pitch;

    // Kamera ayarları
    float m_MovementSpeed = 5.0f;
    float m_MouseSensitivity = 0.1f;
    float m_FOV = 75.0f; // FPS oyunlar için ideal
    float m_NearPlane = 0.1f;
    float m_FarPlane = 2000.0f;
};

} // namespace Archura
