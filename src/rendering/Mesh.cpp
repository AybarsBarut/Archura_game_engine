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
#include <map>
#include <algorithm>
#include <ufbx.h>

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
    // VAO olustur
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

    // Animation Attributes (8, 9 to avoid conflict with Instancing 4-7)
    // Bone IDs
    glEnableVertexAttribArray(8);
    glVertexAttribIPointer(8, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));

    // Weights
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

    glBindVertexArray(0);
}


void Mesh::SetupInstancedAttributes() {
    if (m_InstancedSetup) return;

    glBindVertexArray(m_VAO);
    glGenBuffers(1, &m_InstanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);
    
    // Mat4 4 tane vec4'ten olusur. Attribute location 4, 5, 6, 7 kullanacagiz.
    std::size_t vec4Size = sizeof(glm::vec4);
    
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(4 + i);
        glVertexAttribPointer(4 + i, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(i * vec4Size));
        glVertexAttribDivisor(4 + i, 1); // Her instance icin 1 kez ilerle
    }

    glBindVertexArray(0);
    m_InstancedSetup = true;
}

void Mesh::DrawInstanced(Shader* shader, const std::vector<glm::mat4>& models) {
    if (models.empty()) return;

    if (shader) {
        shader->Bind();
        // Instanced cizimde uModel uniform'u yerine attribute kullanilir. 
        // Shader'in buna uygun olmasi lazim.
        // Eger shader desteklemiyorsa, normal draw fallback yapilabilir ama performans duser.
        // Simdilik Shader'in "uUseInstancing" uniformu oldugunu varsayalim veya
        // Shader kodunu da guncelleyelim.
        shader->SetInt("uUseInstancing", 1); 
    }

    if (!m_InstancedSetup) {
        SetupInstancedAttributes();
    }

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVBO);

    // Buffer verisini guncelle
    // Eger kapasite yeterliyse glBufferSubData, degilse glBufferData
    size_t dataSize = models.size() * sizeof(glm::mat4);
    if (dataSize > m_InstanceCapacity) {
        glBufferData(GL_ARRAY_BUFFER, dataSize, models.data(), GL_DYNAMIC_DRAW);
        m_InstanceCapacity = dataSize;
    } else {
        glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, models.data());
    }

    glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(m_Indices.size()), GL_UNSIGNED_INT, 0, static_cast<unsigned int>(models.size()));
    
    glBindVertexArray(0);
    
    if (shader) {
        shader->SetInt("uUseInstancing", 0); // Reset
    }
}

void Mesh::Draw(Shader* shader) {
    if (shader) {
        shader->Bind();
    }

    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_Indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ==================== Prosedurel Model Olusturucular ====================

Mesh* Mesh::CreateCube(float size) {
    float s = size * 0.5f;
    
    std::vector<Vertex> vertices = {
        // On yuz
        {{-s, -s,  s}, { 0,  0,  1}, {0, 0}, {1, 1, 1}},
        {{ s, -s,  s}, { 0,  0,  1}, {1, 0}, {1, 1, 1}},
        {{ s,  s,  s}, { 0,  0,  1}, {1, 1}, {1, 1, 1}},
        {{-s,  s,  s}, { 0,  0,  1}, {0, 1}, {1, 1, 1}},
        
        // Arka yuz
        {{ s, -s, -s}, { 0,  0, -1}, {0, 0}, {1, 1, 1}},
        {{-s, -s, -s}, { 0,  0, -1}, {1, 0}, {1, 1, 1}},
        {{-s,  s, -s}, { 0,  0, -1}, {1, 1}, {1, 1, 1}},
        {{ s,  s, -s}, { 0,  0, -1}, {0, 1}, {1, 1, 1}},
        
        // Ust yuz
        {{-s,  s,  s}, { 0,  1,  0}, {0, 0}, {1, 1, 1}},
        {{ s,  s,  s}, { 0,  1,  0}, {1, 0}, {1, 1, 1}},
        {{ s,  s, -s}, { 0,  1,  0}, {1, 1}, {1, 1, 1}},
        {{-s,  s, -s}, { 0,  1,  0}, {0, 1}, {1, 1, 1}},
        
        // Alt yuz
        {{-s, -s, -s}, { 0, -1,  0}, {0, 0}, {1, 1, 1}},
        {{ s, -s, -s}, { 0, -1,  0}, {1, 0}, {1, 1, 1}},
        {{ s, -s,  s}, { 0, -1,  0}, {1, 1}, {1, 1, 1}},
        {{-s, -s,  s}, { 0, -1,  0}, {0, 1}, {1, 1, 1}},
        
        // Sag yuz
        {{ s, -s,  s}, { 1,  0,  0}, {0, 0}, {1, 1, 1}},
        {{ s, -s, -s}, { 1,  0,  0}, {1, 0}, {1, 1, 1}},
        {{ s,  s, -s}, { 1,  0,  0}, {1, 1}, {1, 1, 1}},
        {{ s,  s,  s}, { 1,  0,  0}, {0, 1}, {1, 1, 1}},
        
        // Sol yuz
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

Mesh* Mesh::CreatePlane(float width, float height, float uvScale) {
    float w = width * 0.5f;
    float h = height * 0.5f;
    
    std::vector<Vertex> vertices = {
        {{-w, 0, -h}, {0, 1, 0}, {0, 0}, {0.8f, 0.8f, 0.8f}},
        {{ w, 0, -h}, {0, 1, 0}, {1 * uvScale, 0}, {0.8f, 0.8f, 0.8f}},
        {{ w, 0,  h}, {0, 1, 0}, {1 * uvScale, 1 * uvScale}, {0.8f, 0.8f, 0.8f}},
        {{-w, 0,  h}, {0, 1, 0}, {0, 1 * uvScale}, {0.8f, 0.8f, 0.8f}},
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
    
    // Koseler
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
    
    // Indeksler
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

    // Ust Yarikure
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

    // Silindir Govdesi
    // Silindirin ust halkasi ust yarikurenin alt halkasiyla eslesir
    // Silindirin alt halkasi alt yarikurenin ust halkasiyla eslesir
    // Keskin normaller isteseydik silindir govdesi icin koseler ekleyebilirdik,
    // ancak puruzsuz bir kapsul icin koseleri paylasabilir veya uretime devam edebiliriz.
    // UV haritalama ve yapi basitligi icin ozel silindir koseleri uretelim.
    
    for (int s = 0; s <= segments; ++s) {
        float theta = glm::two_pi<float>() * (float)s / (float)segments;
        float x = radius * cos(theta);
        float z = radius * sin(theta);

        // Silindirin ustu
        Vertex vTop;
        vTop.position = glm::vec3(x, halfHeight, z);
        vTop.normal = glm::normalize(glm::vec3(x, 0, z));
        vTop.texCoords = glm::vec2((float)s / segments, 0.5f);
        vTop.color = glm::vec3(1.0f);
        vertices.push_back(vTop);

        // Silindirin alti
        Vertex vBottom;
        vBottom.position = glm::vec3(x, -halfHeight, z);
        vBottom.normal = glm::normalize(glm::vec3(x, 0, z));
        vBottom.texCoords = glm::vec2((float)s / segments, 0.5f);
        vBottom.color = glm::vec3(1.0f);
        vertices.push_back(vBottom);
    }

    // Alt Yarikure
    for (int r = 0; r <= rings; ++r) {
        float phi = glm::half_pi<float>() * (float)r / (float)rings; // 0 to PI/2 (inverted later)
        
        for (int s = 0; s <= segments; ++s) {
            float theta = glm::two_pi<float>() * (float)s / (float)segments;

            float x = radius * cos(phi) * cos(theta);
            float y = -radius * sin(phi); // Alt icin negatif y
            float z = radius * cos(phi) * sin(theta);

            Vertex v;
            v.position = glm::vec3(x, y - halfHeight, z);
            v.normal = glm::normalize(glm::vec3(x, y, z));
            v.texCoords = glm::vec2((float)s / segments, 0.5f + (float)r / (rings * 2 + 1));
            v.color = glm::vec3(1.0f);
            vertices.push_back(v);
        }
    }

    // Indeks Uretimi
    // Cok parcali uretim nedeniyle bu biraz karmasik.
    // Basitlik ve saglamlik icin standart Kure uretimini kullanalim ancak ortayi uzatalim.
    // "Uzatilmis Kure" yaklasimiyla yeniden uygulama:
    
    vertices.clear();
    indices.clear();
    
    int totalRings = rings * 2 + 2; // Ust Kapak + silindir (2 halka) + alt kapak
    // Aslinda, soyle yapalim: Ust Kapak (halkalar), Silindir (2 halka), Alt Kapak (halkalar)
    // Ancak tek bir puruzsuz model yapmak icin:
    // 0'dan halkalara: Ust Yarikure
    // halkalardan halkalar+1'e: Silindir
    // halkalar+1'den 2*halkalar+1'e: Alt Yarikure
    
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

Mesh* Mesh::CreateStairs(float width, float height, float depth, int steps) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float stepHeight = height / steps;
    float stepDepth = depth / steps;
    float halfWidth = width * 0.5f;

    // Her basamak bir kutu (cube) gibidir
    // Basitlik icin her basamagi ayri ayri olusturalim
    // Optimize edilebilir ama bu yeterli
    
    int vertexOffset = 0;

    for (int i = 0; i < steps; ++i) {
        // Basamak boyutlari
        float yBottom = 0.0f; // Hepsi yerden baslasin (dolu merdiven)
        float yTop = (i + 1) * stepHeight;
        float zFront = (i) * stepDepth - (depth * 0.5f);
        float zBack = (i + 1) * stepDepth - (depth * 0.5f);
        
        // 8 Kose
        glm::vec3 p0(-halfWidth, yBottom, zBack);
        glm::vec3 p1( halfWidth, yBottom, zBack);
        glm::vec3 p2( halfWidth, yTop,    zBack);
        glm::vec3 p3(-halfWidth, yTop,    zBack);
        glm::vec3 p4(-halfWidth, yBottom, zFront);
        glm::vec3 p5( halfWidth, yBottom, zFront);
        glm::vec3 p6( halfWidth, yTop,    zFront);
        glm::vec3 p7(-halfWidth, yTop,    zFront);

        // Yuzeyler (Normaller ile)
        // On (Front) - p4, p5, p6, p7
        vertices.push_back({p4, {0,0,1}, {0,0}, {1,1,1}});
        vertices.push_back({p5, {0,0,1}, {1,0}, {1,1,1}});
        vertices.push_back({p6, {0,0,1}, {1,1}, {1,1,1}});
        vertices.push_back({p7, {0,0,1}, {0,1}, {1,1,1}});
        
        // Ust (Top) - p7, p6, p2, p3
        vertices.push_back({p7, {0,1,0}, {0,0}, {1,1,1}});
        vertices.push_back({p6, {0,1,0}, {1,0}, {1,1,1}});
        vertices.push_back({p2, {0,1,0}, {1,1}, {1,1,1}});
        vertices.push_back({p3, {0,1,0}, {0,1}, {1,1,1}});

        // Yanlar (Left/Right)
        // Sag - p5, p1, p2, p6
        vertices.push_back({p5, {1,0,0}, {0,0}, {1,1,1}});
        vertices.push_back({p1, {1,0,0}, {1,0}, {1,1,1}});
        vertices.push_back({p2, {1,0,0}, {1,1}, {1,1,1}});
        vertices.push_back({p6, {1,0,0}, {0,1}, {1,1,1}});

        // Sol - p0, p4, p7, p3
        vertices.push_back({p0, {-1,0,0}, {0,0}, {1,1,1}});
        vertices.push_back({p4, {-1,0,0}, {1,0}, {1,1,1}});
        vertices.push_back({p7, {-1,0,0}, {1,1}, {1,1,1}});
        vertices.push_back({p3, {-1,0,0}, {0,1}, {1,1,1}});

        // Arka ve Alt (Genellikle gorunmez ama ekleyelim)
        // Arka - p1, p0, p3, p2
        vertices.push_back({p1, {0,0,-1}, {0,0}, {1,1,1}});
        vertices.push_back({p0, {0,0,-1}, {1,0}, {1,1,1}});
        vertices.push_back({p3, {0,0,-1}, {1,1}, {1,1,1}});
        vertices.push_back({p2, {0,0,-1}, {0,1}, {1,1,1}});

        // Alt - p0, p1, p5, p4
        vertices.push_back({p0, {0,-1,0}, {0,0}, {1,1,1}});
        vertices.push_back({p1, {0,-1,0}, {1,0}, {1,1,1}});
        vertices.push_back({p5, {0,-1,0}, {1,1}, {1,1,1}});
        vertices.push_back({p4, {0,-1,0}, {0,1}, {1,1,1}});

        // Indeksler (Her yuzey icin 2 ucgen)
        for (int f = 0; f < 6; ++f) {
            int base = vertexOffset + f * 4;
            indices.push_back(base + 0);
            indices.push_back(base + 1);
            indices.push_back(base + 2);
            indices.push_back(base + 0);
            indices.push_back(base + 2);
            indices.push_back(base + 3);
        }
        vertexOffset += 24;
    }

    return new Mesh(vertices, indices);
}

Mesh* Mesh::CreateRamp(float width, float height, float depth) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float w = width * 0.5f;
    float h = height; // Rampanin yuksekligi
    float d = depth * 0.5f;

    // Wedge (Kama) Sekli
    // 6 Kose (Prizma)
    // Taban dikdortgen, ust kenar cizgi
    
    // Arka Yuz (Dikdortgen)
    glm::vec3 p0(-w, 0, -d);
    glm::vec3 p1( w, 0, -d);
    glm::vec3 p2( w, h, -d);
    glm::vec3 p3(-w, h, -d);

    // On Yuz (Cizgi - Yerde)
    glm::vec3 p4(-w, 0, d);
    glm::vec3 p5( w, 0, d);

    // Egimli Yuzey (Rampa) - p4, p5, p2, p3
    // Normali hesapla
    glm::vec3 rampVec = glm::vec3(0, h, -2*d);
    glm::vec3 rampNormal = glm::normalize(glm::cross(rampVec, glm::vec3(1,0,0))); // Basitce

    vertices.push_back({p4, rampNormal, {0,0}, {1,1,1}});
    vertices.push_back({p5, rampNormal, {1,0}, {1,1,1}});
    vertices.push_back({p2, rampNormal, {1,1}, {1,1,1}});
    vertices.push_back({p3, rampNormal, {0,1}, {1,1,1}});

    // Arka Yuz
    vertices.push_back({p1, {0,0,-1}, {0,0}, {1,1,1}});
    vertices.push_back({p0, {0,0,-1}, {1,0}, {1,1,1}});
    vertices.push_back({p3, {0,0,-1}, {1,1}, {1,1,1}});
    vertices.push_back({p2, {0,0,-1}, {0,1}, {1,1,1}});

    // Alt Yuz
    vertices.push_back({p0, {0,-1,0}, {0,0}, {1,1,1}});
    vertices.push_back({p1, {0,-1,0}, {1,0}, {1,1,1}});
    vertices.push_back({p5, {0,-1,0}, {1,1}, {1,1,1}});
    vertices.push_back({p4, {0,-1,0}, {0,1}, {1,1,1}});

    // Yan Yuzler (Ucgen)
    // Sag - p5, p1, p2
    vertices.push_back({p5, {1,0,0}, {0,0}, {1,1,1}});
    vertices.push_back({p1, {1,0,0}, {1,0}, {1,1,1}});
    vertices.push_back({p2, {1,0,0}, {0,1}, {1,1,1}});

    // Sol - p0, p4, p3
    vertices.push_back({p0, {-1,0,0}, {0,0}, {1,1,1}});
    vertices.push_back({p4, {-1,0,0}, {1,0}, {1,1,1}});
    vertices.push_back({p3, {-1,0,0}, {0,1}, {1,1,1}});

    // Indeksler
    // Rampa (Quad)
    indices.push_back(0); indices.push_back(1); indices.push_back(2);
    indices.push_back(0); indices.push_back(2); indices.push_back(3);

    // Arka (Quad)
    indices.push_back(4); indices.push_back(5); indices.push_back(6);
    indices.push_back(4); indices.push_back(6); indices.push_back(7);

    // Alt (Quad)
    indices.push_back(8); indices.push_back(9); indices.push_back(10);
    indices.push_back(8); indices.push_back(10); indices.push_back(11);

    // Sag (Tri)
    indices.push_back(12); indices.push_back(13); indices.push_back(14);

    // Sol (Tri)
    indices.push_back(15); indices.push_back(16); indices.push_back(17);

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
        std::cerr << "Failed to open OBJ file: " << path << "\n";
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
    
    // std::cout << "Loaded OBJ: " << path << " (" << vertices.size() << " vertices)" << std::endl;
    return new Mesh(vertices, indices);
}

Mesh* Mesh::LoadFromFBX(const std::string& path) {
    ufbx_load_opts opts = { 0 }; 
    ufbx_error error;
    ufbx_scene* scene = ufbx_load_file(path.c_str(), &opts, &error);

    if (!scene) {
        std::cerr << "Failed to load FBX: " << path << " Error: " << error.description.data << "\n";
        return nullptr;
    }

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    size_t indexOffset = 0;

    // Sahnedeki tum meshleri tek bir Mesh objesinde birlestirelim
    for (size_t i = 0; i < scene->nodes.count; i++) {
        ufbx_node* node = scene->nodes.data[i];
        if (node->mesh) {
            ufbx_mesh* mesh = node->mesh;
            
            // --- Precompute Skinning Weights ---
            // Map: Original Vertex Index -> List of (BoneID, Weight)
            std::map<int, std::vector<std::pair<int, float>>> vertexWeights;

            if (mesh->skin_deformers.count > 0) {
                ufbx_skin_deformer* skin = mesh->skin_deformers.data[0];
                for (size_t ci = 0; ci < skin->clusters.count; ci++) {
                    ufbx_skin_cluster* cluster = skin->clusters.data[ci];
                    int boneID = (int)ci; // Ideally map bone name to ID using BoneInfo, but for now linear ID

                    // Note: This logic assumes simple linear bone mapping. 
                    // To do it properly, we need to build the skeleton hierarchy first.
                    // But simplified: Just assign IDs.
                    
                    for (size_t vi = 0; vi < cluster->vertices.count; vi++) {
                        int vertIndex = cluster->vertices.data[vi];
                        float weight = (float)cluster->weights.data[vi];
                        vertexWeights[vertIndex].push_back({boneID, weight});
                    }
                }
            }

            // Transformu hesaba katalim mi? Simdilik local space alalim, 
            // ama ideal olarak node->geometry_to_world kullanmaliyiz.
            // Karmasiklik olmamasi icin raw mesh verisini alalim.
            
            // Indeksleri isle
            // ufbx yuzleri (faces) saklar. Genelde ucgenleme gerekir ama ufbx bunu kolaylastirir.
            // Basitce tum yuzleri dolasip ucgenlere bolelim.
            
            // Onceden vertexleri duzlestirmek yerine, her bir face corner icin vertex olusturacagiz.
            // Bu duplicate vertex yaratabilir ama en guvenli yoldur.
            
            // Daha iyi yol: ufbx_triangulate_face
            std::vector<uint32_t> tri_indices;
            tri_indices.resize(mesh->max_face_triangles * 3);

            for (size_t fi = 0; fi < mesh->faces.count; fi++) {
                ufbx_face face = mesh->faces.data[fi];
                size_t num_tris = ufbx_triangulate_face(tri_indices.data(), tri_indices.size(), mesh, face);

                for (size_t ti = 0; ti < num_tris; ti++) {
                    for (int k = 0; k < 3; k++) {
                        uint32_t ix = tri_indices[ti * 3 + k];
                        
                        Vertex v;
                        
                        // Position
                        ufbx_vec3 pos = ufbx_get_vertex_vec3(&mesh->vertex_position, ix);
                        v.position = glm::vec3(pos.x, pos.y, pos.z);

                        // Normal
                        if (mesh->vertex_normal.exists) {
                            ufbx_vec3 norm = ufbx_get_vertex_vec3(&mesh->vertex_normal, ix);
                            v.normal = glm::vec3(norm.x, norm.y, norm.z);
                        }

                        // UV
                        if (mesh->vertex_uv.exists) {
                            ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, ix);
                            v.texCoords = glm::vec2(uv.x, uv.y);
                        }

                        if (mesh->vertex_color.exists) {
                            ufbx_vec4 col = ufbx_get_vertex_vec4(&mesh->vertex_color, ix);
                            v.color = glm::vec3(col.x, col.y, col.z);
                        }
                        
                        // Weights
                        // ix is the control point index (index into vertex_position), which matches cluster indices.
                        if (vertexWeights.count((int)ix)) {
                            auto& weights = vertexWeights[(int)ix];
                            // Sort by weight descending
                            std::sort(weights.begin(), weights.end(), [](const auto& a, const auto& b){ return a.second > b.second; });
                            
                            for (int w = 0; w < std::min((int)weights.size(), MAX_BONE_INFLUENCE); w++) {
                                v.m_BoneIDs[w] = weights[w].first;
                                v.m_Weights[w] = weights[w].second;
                            }
                        }

                        vertices.push_back(v);
                        indices.push_back((unsigned int)(indexOffset + vertices.size() - 1));
                    }
                }
            }
            // indexOffset += ... (Tek dongude hallettik, indisler surekli artiyor)
        }
    }

    ufbx_free_scene(scene);

    if (vertices.empty()) {
        std::cerr << "FBX loaded but no mesh data found: " << path << "\n";
        return nullptr;
    }

    // std::cout << "Loaded FBX: " << path << " (" << vertices.size() << " vertices)" << std::endl;
    return new Mesh(vertices, indices);
}


} // namespace Archura
