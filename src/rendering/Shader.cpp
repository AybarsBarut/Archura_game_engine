#include "Shader.h"
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Archura {

Shader::~Shader() {
    if (m_ProgramID) {
        glDeleteProgram(m_ProgramID);
    }
}

bool Shader::LoadFromSource(const std::string& vertexSrc, const std::string& fragmentSrc) {
    // Vertex shader compile
    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    if (vertexShader == 0) return false;

    // Fragment shader compile
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    // Program link
    bool success = LinkProgram(vertexShader, fragmentShader);

    // Shader'lari temizle (artik programa baglandi)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return success;
}

bool Shader::LoadFromFile(const std::string& vertexPath, const std::string& fragmentPath) {
    // Vertex shader dosyasini oku
    std::ifstream vertexFile(vertexPath);
    if (!vertexFile.is_open()) {
        std::cerr << "Failed to open vertex shader: " << vertexPath << std::endl;
        return false;
    }
    std::stringstream vertexStream;
    vertexStream << vertexFile.rdbuf();
    std::string vertexSrc = vertexStream.str();

    // Fragment shader dosyasini oku
    std::ifstream fragmentFile(fragmentPath);
    if (!fragmentFile.is_open()) {
        std::cerr << "Failed to open fragment shader: " << fragmentPath << std::endl;
        return false;
    }
    std::stringstream fragmentStream;
    fragmentStream << fragmentFile.rdbuf();
    std::string fragmentSrc = fragmentStream.str();

    return LoadFromSource(vertexSrc, fragmentSrc);
}

void Shader::Bind() const {
    glUseProgram(m_ProgramID);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

void Shader::SetInt(const std::string& name, int value) {
    glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetFloat(const std::string& name, float value) {
    glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetVec2(const std::string& name, const glm::vec2& value) {
    glUniform2fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) {
    glUniform4fv(GetUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::SetMat3(const std::string& name, const glm::mat3& value) {
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Derleme hatasini kontrol et
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error (" 
                  << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") 
                  << "):\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool Shader::LinkProgram(unsigned int vertexShader, unsigned int fragmentShader) {
    m_ProgramID = glCreateProgram();
    glAttachShader(m_ProgramID, vertexShader);
    glAttachShader(m_ProgramID, fragmentShader);
    glLinkProgram(m_ProgramID);

    // Baglama hatasini kontrol et
    int success;
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_ProgramID, 512, nullptr, infoLog);
        std::cerr << "Shader linking error:\n" << infoLog << std::endl;
        glDeleteProgram(m_ProgramID);
        m_ProgramID = 0;
        return false;
    }

    return true;
}

int Shader::GetUniformLocation(const std::string& name) {
    // Onbellekte var mi kontrol et
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
        return m_UniformLocationCache[name];
    }

    // OpenGL'den konumu al ve onbellege al
    int location = glGetUniformLocation(m_ProgramID, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' not found!" << std::endl;
    }
    m_UniformLocationCache[name] = location;
    return location;
}

} // namespace Archura
