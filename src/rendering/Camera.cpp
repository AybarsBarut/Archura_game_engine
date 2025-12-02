#include "Camera.h"
#include <algorithm>

namespace Archura {

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch)
    : m_Position(position)
    , m_WorldUp(up)
    , m_Yaw(yaw)
    , m_Pitch(pitch)
    , m_Front(glm::vec3(0.0f, 0.0f, -1.0f))
{
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::GetProjectionMatrix(float aspectRatio) const {
    return glm::perspective(glm::radians(m_FOV), aspectRatio, m_NearPlane, m_FarPlane);
}

void Camera::ProcessKeyboard(MovementDirection direction, float deltaTime) {
    float velocity = m_MovementSpeed * deltaTime;

    switch (direction) {
        case MovementDirection::Forward:
            m_Position += m_Front * velocity;
            break;
        case MovementDirection::Backward:
            m_Position -= m_Front * velocity;
            break;
        case MovementDirection::Left:
            m_Position -= m_Right * velocity;
            break;
        case MovementDirection::Right:
            m_Position += m_Right * velocity;
            break;
        case MovementDirection::Up:
            m_Position += m_WorldUp * velocity;
            break;
        case MovementDirection::Down:
            m_Position -= m_WorldUp * velocity;
            break;
    }
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch) {
    xOffset *= m_MouseSensitivity;
    yOffset *= m_MouseSensitivity;

    m_Yaw += xOffset;
    m_Pitch += yOffset;

    // Pitch'i sınırla (baş aşağı bakışı önle)
    if (constrainPitch) {
        m_Pitch = std::clamp(m_Pitch, -89.0f, 89.0f);
    }

    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset) {
    m_FOV -= yOffset;
    m_FOV = std::clamp(m_FOV, 1.0f, 120.0f);
}

void Camera::UpdateCameraVectors() {
    // Yeni Front vektörünü hesapla
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);

    // Right ve Up vektörlerini yeniden hesapla
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

} // namespace Archura
