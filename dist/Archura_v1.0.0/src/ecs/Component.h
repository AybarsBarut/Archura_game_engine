#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <glm/glm.hpp>
#include <string> // Added for std::string in ScriptComponent

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
    class Texture* texture = nullptr; // Skin/Texture
    glm::vec3 color = glm::vec3(1.0f);
};

/**
 * @brief Box Collider component - Fiziksel carpisma kutusu
 */
struct BoxCollider : public Component {
    glm::vec3 size = glm::vec3(1.0f); // Boyutlar (Genislik, Yukseklik, Derinlik)
    glm::vec3 center = glm::vec3(0.0f); // Merkez ofseti
};

/**
 * @brief Health component - Can ve hasar durumu
 */
struct Health : public Component {
    float current = 100.0f;
    float max = 100.0f;
    bool isDead = false;
};

/**
 * @brief RigidBody component - Fiziksel hareket ve etkilesim
 */
struct RigidBody : public Component {
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 force = glm::vec3(0.0f);
    float mass = 1.0f;
    float drag = 0.1f;
    bool useGravity = true;
    bool isKinematic = false; // If true, physics doesn't move it (user does)
};

/**
 * @brief Script component - Harici script'leri baglamak icin
 */
struct ScriptComponent : public Component {
    std::string className; // Name of the C# class
    // Placeholder for script instance ID or pointer
    int scriptInstanceID = -1;
};

/**
 * @brief Spawn Point component - Oyuncu dogus noktasi
 */
struct SpawnPoint : public Component {
    int teamId = 0; // 1 = Team A, 2 = Team B
};

} // namespace Archura
