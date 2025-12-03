#include "Mesh.h"
#include "Shader.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace Archura {

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : m_Vertices(vertices)
    , m_Indices(indices)
    , m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
{
    SetupMesh();
}

Mesh::~Mesh() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
}

void Mesh::SetupMesh() {
    // VAO oluştur
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);

    // VBO - Vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_STATIC_DRAW);

    // EBO - Index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), m_Indices.data(), GL_STATIC_DRAW);

    // Vertex attributes
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    // Color
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    glBindVertexArray(0);
}

void Mesh::Draw(Shader* shader) {
    if (shader) {
        shader->Bind();
    }

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_Indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ==================== Procedural Mesh Generators ====================

Mesh* Mesh::CreateCube(float size) {
    float s = size * 0.5f;
    
    std::vector<Vertex> vertices = {
        // Front face
        {{-s, -s,  s}, { 0,  0,  1}, {0, 0}, {1, 1, 1}},
        {{ s, -s,  s}, { 0,  0,  1}, {1, 0}, {1, 1, 1}},
        {{ s,  s,  s}, { 0,  0,  1}, {1, 1}, {1, 1, 1}},
        {{-s,  s,  s}, { 0,  0,  1}, {0, 1}, {1, 1, 1}},
        
        // Back face
        {{ s, -s, -s}, { 0,  0, -1}, {0, 0}, {1, 1, 1}},
        {{-s, -s, -s}, { 0,  0, -1}, {1, 0}, {1, 1, 1}},
        {{-s,  s, -s}, { 0,  0, -1}, {1, 1}, {1, 1, 1}},
        {{ s,  s, -s}, { 0,  0, -1}, {0, 1}, {1, 1, 1}},
        
        // Top face
        {{-s,  s,  s}, { 0,  1,  0}, {0, 0}, {1, 1, 1}},
        {{ s,  s,  s}, { 0,  1,  0}, {1, 0}, {1, 1, 1}},
        {{ s,  s, -s}, { 0,  1,  0}, {1, 1}, {1, 1, 1}},
        {{-s,  s, -s}, { 0,  1,  0}, {0, 1}, {1, 1, 1}},
        
        // Bottom face
        {{-s, -s, -s}, { 0, -1,  0}, {0, 0}, {1, 1, 1}},
        {{ s, -s, -s}, { 0, -1,  0}, {1, 0}, {1, 1, 1}},
        {{ s, -s,  s}, { 0, -1,  0}, {1, 1}, {1, 1, 1}},
        {{-s, -s,  s}, { 0, -1,  0}, {0, 1}, {1, 1, 1}},
        
        // Right face
        {{ s, -s,  s}, { 1,  0,  0}, {0, 0}, {1, 1, 1}},
        {{ s, -s, -s}, { 1,  0,  0}, {1, 0}, {1, 1, 1}},
        {{ s,  s, -s}, { 1,  0,  0}, {1, 1}, {1, 1, 1}},
        {{ s,  s,  s}, { 1,  0,  0}, {0, 1}, {1, 1, 1}},
        
        // Left face
        {{-s, -s, -s}, {-1,  0,  0}, {0, 0}, {1, 1, 1}},
        {{-s, -s,  s}, {-1,  0,  0}, {1, 0}, {1, 1, 1}},
        {{-s,  s,  s}, {-1,  0,  0}, {1, 1}, {1, 1, 1}},
        {{-s,  s, -s}, {-1,  0,  0}, {0, 1}, {1, 1, 1}},
    };
    
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < 6; ++i) {
        unsigned int base = i * 4;
        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }
    
    return new Mesh(vertices, indices);
}

Mesh* Mesh::CreatePlane(float width, float height) {
    float w = width * 0.5f;
    float h = height * 0.5f;
    
    std::vector<Vertex> vertices = {
        {{-w, 0, -h}, {0, 1, 0}, {0, 0}, {0.8f, 0.8f, 0.8f}},
        {{ w, 0, -h}, {0, 1, 0}, {1, 0}, {0.8f, 0.8f, 0.8f}},
        {{ w, 0,  h}, {0, 1, 0}, {1, 1}, {0.8f, 0.8f, 0.8f}},
        {{-w, 0,  h}, {0, 1, 0}, {0, 1}, {0.8f, 0.8f, 0.8f}},
    };
    
    std::vector<unsigned int> indices = {
        0, 1, 2,
        0, 2, 3
    };
    
    return new Mesh(vertices, indices);
}

Mesh* Mesh::CreateSphere(float radius, int segments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    const float PI = 3.14159265359f;
    const int rings = segments / 2;
    const int sectors = segments;
    
    // Vertices
    for (int r = 0; r <= rings; ++r) {
        float phi = PI * (float)r / (float)rings;
        
        for (int s = 0; s <= sectors; ++s) {
            float theta = 2.0f * PI * (float)s / (float)sectors;
            
            Vertex vertex;
            vertex.position.x = radius * sin(phi) * cos(theta);
            vertex.position.y = radius * cos(phi);
            vertex.position.z = radius * sin(phi) * sin(theta);
            
            vertex.normal = glm::normalize(vertex.position);
            vertex.texCoords = glm::vec2((float)s / sectors, (float)r / rings);
            vertex.color = glm::vec3(1.0f);
            
            vertices.push_back(vertex);
        }
    }
    
    // Indices
    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < sectors; ++s) {
            unsigned int current = r * (sectors + 1) + s;
            unsigned int next = current + sectors + 1;
            
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);
            
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }
    
    return new Mesh(vertices, indices);
}

Mesh* Mesh::CreateCapsule(float radius, float height) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    int segments = 16;
    int rings = 8;
    float cylinderHeight = height - 2 * radius;
    if (cylinderHeight < 0) cylinderHeight = 0;
    float halfHeight = cylinderHeight * 0.5f;

    // Top Hemisphere
    for (int r = 0; r <= rings; ++r) {
        float phi = glm::half_pi<float>() * (float)r / (float)rings; // 0 to PI/2
        
        for (int s = 0; s <= segments; ++s) {
            float theta = glm::two_pi<float>() * (float)s / (float)segments;

            float x = radius * cos(phi) * cos(theta);
            float y = radius * sin(phi);
            float z = radius * cos(phi) * sin(theta);

            Vertex v;
            v.position = glm::vec3(x, y + halfHeight, z);
            v.normal = glm::normalize(glm::vec3(x, y, z)); // Normal is relative to sphere center
            v.texCoords = glm::vec2((float)s / segments, (float)r / (rings * 2 + 1)); // Simplified UV
            v.color = glm::vec3(1.0f);
            vertices.push_back(v);
        }
    }

    // Cylinder Body
    // Top ring of cylinder matches bottom ring of top hemisphere
    // Bottom ring of cylinder matches top ring of bottom hemisphere
    // We can just add vertices for the cylinder body to ensure sharp normals if we wanted, 
    // but for a smooth capsule, we can share vertices or just continue the generation.
    // Let's generate specific cylinder vertices for simplicity in UV mapping and structure.
    
    for (int s = 0; s <= segments; ++s) {
        float theta = glm::two_pi<float>() * (float)s / (float)segments;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        // Top of cylinder
        Vertex vTop;
        vTop.position = glm::vec3(x, halfHeight, z);
        vTop.normal = glm::normalize(glm::vec3(x, 0, z));
        vTop.texCoords = glm::vec2((float)s / segments, 0.5f);
        vTop.color = glm::vec3(1.0f);
        vertices.push_back(vTop);

        // Bottom of cylinder
        Vertex vBottom;
        vBottom.position = glm::vec3(x, -halfHeight, z);
        vBottom.normal = glm::normalize(glm::vec3(x, 0, z));
        vBottom.texCoords = glm::vec2((float)s / segments, 0.5f);
        vBottom.color = glm::vec3(1.0f);
        vertices.push_back(vBottom);
    }

    // Bottom Hemisphere
    for (int r = 0; r <= rings; ++r) {
        float phi = glm::half_pi<float>() * (float)r / (float)rings; // 0 to PI/2 (inverted later)
        
        for (int s = 0; s <= segments; ++s) {
            float theta = glm::two_pi<float>() * (float)s / (float)segments;

            float x = radius * cos(phi) * cos(theta);
            float y = -radius * sin(phi); // Negative y for bottom
            float z = radius * cos(phi) * sin(theta);

            Vertex v;
            v.position = glm::vec3(x, y - halfHeight, z);
            v.normal = glm::normalize(glm::vec3(x, y, z));
            v.texCoords = glm::vec2((float)s / segments, 0.5f + (float)r / (rings * 2 + 1));
            v.color = glm::vec3(1.0f);
            vertices.push_back(v);
        }
    }

    // Indices Generation
    // This is a bit complex due to the multi-part generation. 
    // For simplicity and robustness, let's use a standard Sphere generation but stretch the middle.
    // Re-implementation with "Stretched Sphere" approach:
    
    vertices.clear();
    indices.clear();
    
    int totalRings = rings * 2 + 2; // Top cap + cylinder (2 rings) + bottom cap
    // Actually, let's do: Top Cap (rings), Cylinder (2 rings), Bottom Cap (rings)
    // But to make it one smooth mesh:
    // 0 to rings: Top Hemisphere
    // rings to rings+1: Cylinder
    // rings+1 to 2*rings+1: Bottom Hemisphere
    
    for (int r = 0; r <= rings; ++r) { // Top Hemisphere
        float phi = glm::half_pi<float>() * (1.0f - (float)r / rings); // PI/2 to 0
        for (int s = 0; s <= segments; ++s) {
            float theta = glm::two_pi<float>() * (float)s / segments;
            float x = radius * cos(phi) * cos(theta);
            float y = radius * sin(phi);
            float z = radius * cos(phi) * sin(theta);
            
            Vertex v;
            v.position = glm::vec3(x, y + halfHeight, z);
            v.normal = glm::normalize(glm::vec3(x, y, z));
            v.texCoords = glm::vec2((float)s / segments, (float)r / (totalRings)); 
            v.color = glm::vec3(1.0f);
            vertices.push_back(v);
        }
    }
    
    for (int r = 0; r <= rings; ++r) { // Bottom Hemisphere
        float phi = glm::half_pi<float>() * ((float)r / rings); // 0 to PI/2 (going down)
        for (int s = 0; s <= segments; ++s) {
            float theta = glm::two_pi<float>() * (float)s / segments;
            float x = radius * cos(phi) * cos(theta);
            float y = -radius * sin(phi);
            float z = radius * cos(phi) * sin(theta);
            
            Vertex v;
            v.position = glm::vec3(x, y - halfHeight, z);
            v.normal = glm::normalize(glm::vec3(x, y, z));
            v.texCoords = glm::vec2((float)s / segments, (float)(r + rings) / (totalRings));
            v.color = glm::vec3(1.0f);
            vertices.push_back(v);
        }
    }
    
    // Indices
    int verticesPerRing = segments + 1;
    int ringCount = vertices.size() / verticesPerRing;
    
    for (int r = 0; r < ringCount - 1; ++r) {
        for (int s = 0; s < segments; ++s) {
            int current = r * verticesPerRing + s;
            int next = current + verticesPerRing;
            
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);
            
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    return new Mesh(vertices, indices);
}

Mesh* Mesh::LoadFromOBJ(const std::string& path) {
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec3> temp_normals;
    std::vector<glm::vec2> temp_texCoords;
    
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return nullptr;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string prefix;
        ss >> prefix;
        
        if (prefix == "v") {
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        }
        else if (prefix == "vt") {
            glm::vec2 tex;
            ss >> tex.x >> tex.y;
            temp_texCoords.push_back(tex);
        }
        else if (prefix == "vn") {
            glm::vec3 norm;
            ss >> norm.x >> norm.y >> norm.z;
            temp_normals.push_back(norm);
        }
        else if (prefix == "f") {
            std::string vertexStr;
            int vertexCount = 0;
            
            while (ss >> vertexStr) {
                if (vertexCount >= 3) continue;

                unsigned int vIndex = 0, vtIndex = 0, vnIndex = 0;
                
                size_t firstSlash = vertexStr.find('/');
                size_t secondSlash = vertexStr.find('/', firstSlash + 1);
                
                if (firstSlash != std::string::npos) {
                    try { vIndex = std::stoi(vertexStr.substr(0, firstSlash)); } catch (...) { vIndex = 0; }
                    
                    if (secondSlash != std::string::npos) {
                        if (secondSlash > firstSlash + 1) {
                            try { vtIndex = std::stoi(vertexStr.substr(firstSlash + 1, secondSlash - firstSlash - 1)); } catch (...) { vtIndex = 0; }
                        }
                        try { vnIndex = std::stoi(vertexStr.substr(secondSlash + 1)); } catch (...) { vnIndex = 0; }
                    } else {
                        try { vtIndex = std::stoi(vertexStr.substr(firstSlash + 1)); } catch (...) { vtIndex = 0; }
                    }
                } else {
                    try { vIndex = std::stoi(vertexStr); } catch (...) { vIndex = 0; }
                }
                
                Vertex vertex;
                if (vIndex > 0 && vIndex <= temp_positions.size()) vertex.position = temp_positions[vIndex - 1];
                if (vtIndex > 0 && vtIndex <= temp_texCoords.size()) vertex.texCoords = temp_texCoords[vtIndex - 1];
                if (vnIndex > 0 && vnIndex <= temp_normals.size()) vertex.normal = temp_normals[vnIndex - 1];
                
                vertices.push_back(vertex);
                indices.push_back((unsigned int)vertices.size() - 1);
                
                vertexCount++;
            }
        }
    }
    
    if (vertices.empty()) return nullptr;
    
    std::cout << "Loaded OBJ: " << path << " (" << vertices.size() << " vertices)" << std::endl;
    return new Mesh(vertices, indices);
}

} // namespace Archura
