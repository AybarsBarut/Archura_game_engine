#include "Texture.h"
#include <glad/glad.h>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Archura {

Texture::~Texture() {
    if (m_TextureID) {
        glDeleteTextures(1, &m_TextureID);
    }
}

bool Texture::LoadFromFile(const std::string& path, bool generateMipmaps) {
    m_Path = path;

    // stb_image ile resim yukle
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
    
    if (!data) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        std::cerr << "STB Error: " << stbi_failure_reason() << std::endl;
        return false;
    }

    // OpenGL dokusu olustur
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // Doku parametreleri
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Bicim belirle
    GLenum format = GL_RGB;
    if (m_Channels == 1) format = GL_RED;
    else if (m_Channels == 3) format = GL_RGB;
    else if (m_Channels == 4) format = GL_RGBA;

    // Doku verisini GPU'ya yukle
    glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
    
    if (generateMipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Bellegi serbest birak
    stbi_image_free(data);

    std::cout << "Loaded texture: " << path << " (" << m_Width << "x" << m_Height << ", " << m_Channels << " channels)" << std::endl;
    
    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

bool Texture::CreateSolid(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    m_Width = 1;
    m_Height = 1;
    m_Channels = 4;

    unsigned char data[] = { r, g, b, a };

    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);
    return true;
}

void Texture::Bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

// ==================== TextureManager ====================

TextureManager& TextureManager::Get() {
    static TextureManager instance;
    return instance;
}

Texture* TextureManager::Load(const std::string& name, const std::string& path, bool generateMipmaps) {
    // Zaten var mi kontrol et
    auto it = m_Textures.find(name);
    if (it != m_Textures.end()) {
        return it->second;
    }

    // Yeni doku olustur
    Texture* texture = new Texture();
    if (texture->LoadFromFile(path, generateMipmaps)) {
        m_Textures[name] = texture;
        return texture;
    }

    delete texture;
    return nullptr;
}

Texture* TextureManager::Get(const std::string& name) {
    auto it = m_Textures.find(name);
    if (it != m_Textures.end()) {
        return it->second;
    }
    return nullptr;
}

void TextureManager::Clear() {
    for (auto& pair : m_Textures) {
        delete pair.second;
    }
    m_Textures.clear();
}

} // namespace Archura
