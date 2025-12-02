#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <glm/glm.hpp>

namespace Archura {

// Forward declarations
class Entity;
class System;

/**
 * @brief Component base class - Tüm component'ler bundan türer
 */
struct Component {
    virtual ~Component() = default;
};

/**
 * @brief Transform component - Pozisyon, rotasyon, scale
 */
struct Transform : public Component {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // Euler angles (degrees)
    glm::vec3 scale = glm::vec3(1.0f);

    glm::mat4 GetModelMatrix() const;
};

/**
 * @brief Mesh Renderer component
 */
struct MeshRenderer : public Component {
    class Mesh* mesh = nullptr;
    class Shader* shader = nullptr;
    glm::vec3 color = glm::vec3(1.0f);
};

} // namespace Archura
