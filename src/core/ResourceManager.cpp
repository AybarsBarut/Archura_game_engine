#include "ResourceManager.h"
#include "../rendering/Shader.h"
#include "../rendering/Texture.h"
#include "../rendering/Mesh.h"
#include <iostream>

namespace Archura {

ResourceManager& ResourceManager::Get() {
    static ResourceManager instance;
    return instance;
}

// ==================== Shader Yonetimi ====================

Shader* ResourceManager::LoadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath) {
    // Zaten var mi?
    auto it = m_Shaders.find(name);
    if (it != m_Shaders.end()) {
        // std::cout << "Shader '" << name << "' already loaded, returning cached version." << std::endl;
        return it->second;
    }

    // Yeni shader yukle
    Shader* shader = new Shader();
    if (shader->LoadFromFile(vertPath, fragPath)) {
        m_Shaders[name] = shader;
        // std::cout << "Loaded shader: " << name << std::endl;
        return shader;
    }

    delete shader;
    std::cerr << "Failed to load shader: " << name << std::endl;
    return nullptr;
}

Shader* ResourceManager::GetShader(const std::string& name) {
    auto it = m_Shaders.find(name);
    return (it != m_Shaders.end()) ? it->second : nullptr;
}

// ==================== Doku Yonetimi ====================

Texture* ResourceManager::LoadTexture(const std::string& name, const std::string& path, bool generateMipmaps) {
    auto it = m_Textures.find(name);
    if (it != m_Textures.end()) {
        return it->second;
    }

    Texture* texture = new Texture();
    if (texture->LoadFromFile(path, generateMipmaps)) {
        m_Textures[name] = texture;
        return texture;
    }

    delete texture;
    return nullptr;
}

Texture* ResourceManager::GetTexture(const std::string& name) {
    auto it = m_Textures.find(name);
    return (it != m_Textures.end()) ? it->second : nullptr;
}

// ==================== Model Yonetimi ====================

Mesh* ResourceManager::AddMesh(const std::string& name, Mesh* mesh) {
    if (!mesh) return nullptr;

    auto it = m_Meshes.find(name);
    if (it != m_Meshes.end()) {
        // std::cout << "Mesh '" << name << "' already exists, replacing." << std::endl;
        delete it->second;
    }

    m_Meshes[name] = mesh;
    return mesh;
}

Mesh* ResourceManager::GetMesh(const std::string& name) {
    auto it = m_Meshes.find(name);
    return (it != m_Meshes.end()) ? it->second : nullptr;
}

// ==================== Temizlik ====================

void ResourceManager::Clear() {
    ClearShaders();
    ClearTextures();
    ClearMeshes();
    std::cout << "ResourceManager cleared all resources." << std::endl;
}

void ResourceManager::ClearShaders() {
    for (auto& pair : m_Shaders) {
        delete pair.second;
    }
    m_Shaders.clear();
}

void ResourceManager::ClearTextures() {
    for (auto& pair : m_Textures) {
        delete pair.second;
    }
    m_Textures.clear();
}

void ResourceManager::ClearMeshes() {
    for (auto& pair : m_Meshes) {
        delete pair.second;
    }
    m_Meshes.clear();
}

} // namespace Archura
