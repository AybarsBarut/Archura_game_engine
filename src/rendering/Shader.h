#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Archura {

/**
 * @brief Shader sınıfı - GLSL shader derleme ve yönetimi
 * 
 * Vertex ve Fragment shader'ları yükler, derler ve uniform'ları yönetir
 */
class Shader {
public:
    Shader() = default;
    ~Shader();

    // Shader'ı source code'dan yükle
    bool LoadFromSource(const std::string& vertexSrc, const std::string& fragmentSrc);
    
    // Shader'ı dosyadan yükle
    bool LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath);

    void Bind() const;
    void Unbind() const;

    // Uniform setters
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetVec2(const std::string& name, const glm::vec2& value);
    void SetVec3(const std::string& name, const glm::vec3& value);
    void SetVec4(const std::string& name, const glm::vec4& value);
    void SetMat3(const std::string& name, const glm::mat3& value);
    void SetMat4(const std::string& name, const glm::mat4& value);

    unsigned int GetProgramID() const { return m_ProgramID; }

private:
    unsigned int CompileShader(unsigned int type, const std::string& source);
    bool LinkProgram(unsigned int vertexShader, unsigned int fragmentShader);
    int GetUniformLocation(const std::string& name);

private:
    unsigned int m_ProgramID = 0;
    std::unordered_map<std::string, int> m_UniformLocationCache;
};

} // namespace Archura
