#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Archura {

class Shader;

/**
 * @brief Vertex yapısı - mesh'in her bir vertex'i için data
 */
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 color;

    Vertex(
        const glm::vec3& pos = glm::vec3(0.0f),
        const glm::vec3& norm = glm::vec3(0.0f, 1.0f, 0.0f),
        const glm::vec2& tex = glm::vec2(0.0f),
        const glm::vec3& col = glm::vec3(1.0f)
    ) : position(pos), normal(norm), texCoords(tex), color(col) {}
};

/**
 * @brief Mesh sınıfı - 3D geometri yönetimi
 * 
 * VAO/VBO/EBO ile optimize edilmiş rendering
 */
class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    // Copy constructor ve assignment operator'ı disable et
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    void Draw(Shader* shader);

    // Procedural mesh oluşturucular
    static Mesh* CreateCube(float size = 1.0f);
    static Mesh* CreatePlane(float width = 10.0f, float height = 10.0f);
    static Mesh* CreateSphere(float radius = 1.0f, int segments = 32);
    static Mesh* CreateCapsule(float radius = 0.5f, float height = 2.0f);
    
    // Model Loader
    static Mesh* LoadFromOBJ(const std::string& path);

private:
    void SetupMesh();

private:
    std::vector<Vertex> m_Vertices;
    std::vector<unsigned int> m_Indices;

    unsigned int m_VAO;  // Vertex Array Object
    unsigned int m_VBO;  // Vertex Buffer Object
    unsigned int m_EBO;  // Element Buffer Object
};

} // namespace Archura
