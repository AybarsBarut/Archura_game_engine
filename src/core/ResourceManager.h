#pragma once

#include <string>
#include <unordered_map>
#include <memory>

namespace Archura {

class Shader;
class Texture;
class Mesh;

/**
 * @brief ResourceManager - Unified asset management
 * 
 * Tüm game asset'lerini merkezi olarak yönetir
 */
class ResourceManager {
public:
    static ResourceManager& Get();

    // Shader management
    Shader* LoadShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);
    Shader* GetShader(const std::string& name);

    // Texture management
    Texture* LoadTexture(const std::string& name, const std::string& path, bool generateMipmaps = true);
    Texture* GetTexture(const std::string& name);

    // Mesh management (procedural veya loaded)
    Mesh* AddMesh(const std::string& name, Mesh* mesh);
    Mesh* GetMesh(const std::string& name);

    // Resource lifecycle
    void Clear();
    void ClearShaders();
    void ClearTextures();
    void ClearMeshes();

    // Stats
    size_t GetShaderCount() const { return m_Shaders.size(); }
    size_t GetTextureCount() const { return m_Textures.size(); }
    size_t GetMeshCount() const { return m_Meshes.size(); }

private:
    ResourceManager() = default;
    ~ResourceManager() { Clear(); }

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

private:
    std::unordered_map<std::string, Shader*> m_Shaders;
    std::unordered_map<std::string, Texture*> m_Textures;
    std::unordered_map<std::string, Mesh*> m_Meshes;
};

} // namespace Archura
