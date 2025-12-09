#pragma once

#include <glm/glm.hpp>
#include <memory>

namespace Archura {

class Shader;

/**
 * @brief Ana Renderer sınıfı - OpenGL rendering yönetimi
 * 
 * GTX 1050 için optimize edilmiş rendering pipeline
 */
class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    bool Init();
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    void SetClearColor(const glm::vec4& color);
    void Clear();

    // Stats
    struct RenderStats {
        uint32_t drawCalls = 0;
        uint32_t triangles = 0;
        uint32_t vertices = 0;
        
        void Reset() {
            drawCalls = 0;
            triangles = 0;
            vertices = 0;
        }
    };

    const RenderStats& GetStats() const { return m_Stats; }

private:
    glm::vec4 m_ClearColor = glm::vec4(0.1f, 0.1f, 0.15f, 1.0f); // Koyu gri-mavi
    RenderStats m_Stats;
};

} // namespace Archura
