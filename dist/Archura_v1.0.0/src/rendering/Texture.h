#pragma once

#include <string>
#include <unordered_map>

namespace Archura {

/**
 * @brief Texture sınıfı - 2D texture yönetimi
 * 
 * stb_image ile texture loading, OpenGL texture oluşturma
 */
class Texture {
public:
    Texture() = default;
    ~Texture();

    // Texture'ı dosyadan yükle
    bool LoadFromFile(const std::string& path, bool generateMipmaps = true);
    
    // Solid color texture oluştur
    bool CreateSolid(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;

    unsigned int GetID() const { return m_TextureID; }
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    int GetChannels() const { return m_Channels; }

    bool IsLoaded() const { return m_TextureID != 0; }

private:
    unsigned int m_TextureID = 0;
    int m_Width = 0;
    int m_Height = 0;
    int m_Channels = 0;
    std::string m_Path;
};

/**
 * @brief TextureManager - Texture caching ve resource management
 */
class TextureManager {
public:
    static TextureManager& Get();

    // Texture yükle ve cache'le
    Texture* Load(const std::string& name, const std::string& path, bool generateMipmaps = true);
    
    // Cache'den texture al
    Texture* Get(const std::string& name);
    
    // Tüm texture'ları temizle
    void Clear();

private:
    TextureManager() = default;
    ~TextureManager() { Clear(); }

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

private:
    std::unordered_map<std::string, Texture*> m_Textures;
};

} // namespace Archura
