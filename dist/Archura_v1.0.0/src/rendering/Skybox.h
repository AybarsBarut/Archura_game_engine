#pragma once

#include "Shader.h"
#include "Camera.h"
#include <memory>

namespace Archura {

class Skybox {
public:
    Skybox();
    ~Skybox();

    void Init();
    void Draw(const Camera& camera, float aspectRatio);

private:
    unsigned int m_VAO = 0;
    unsigned int m_VBO = 0;
    std::unique_ptr<Shader> m_Shader;
};

} // namespace Archura
