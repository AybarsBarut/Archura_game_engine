#include "Skybox.h"
#include <glad/glad.h>
#include <vector>

namespace Archura {

Skybox::Skybox() {}

Skybox::~Skybox() {
    if (m_VAO != 0) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0) glDeleteBuffers(1, &m_VBO);
}

void Skybox::Init() {
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    const std::string vertexShaderSource = R"(
        #version 330 core
        layout (location = 0) in vec3 aPos;

        out vec3 TexCoords;

        uniform mat4 projection;
        uniform mat4 view;

        void main()
        {
            TexCoords = aPos;
            vec4 pos = projection * view * vec4(aPos, 1.0);
            gl_Position = pos.xyww;
        }
    )";

    const std::string fragmentShaderSource = R"(
        #version 330 core
        out vec4 FragColor;

        in vec3 TexCoords;

        void main()
        {
            vec3 dir = normalize(TexCoords);
            
            // Simple gradient sky
            vec3 topColor = vec3(0.2, 0.5, 0.9); // Sky blue
            vec3 bottomColor = vec3(0.8, 0.9, 1.0); // Horizon white/blue
            
            float t = 0.5 * (dir.y + 1.0);
            t = clamp(t, 0.0, 1.0);
            
            // More intense blue at top
            vec3 color = mix(bottomColor, topColor, max(dir.y, 0.0));
            
            if (dir.y < 0.0) {
                 color = vec3(0.3, 0.3, 0.3); // Darker below horizon
            }

            FragColor = vec4(color, 1.0);
        }
    )";

    m_Shader = std::make_unique<Shader>();
    m_Shader->LoadFromSource(vertexShaderSource, fragmentShaderSource);
}

void Skybox::Draw(const Camera& camera, float aspectRatio) {
    glDepthFunc(GL_LEQUAL);
    m_Shader->Bind();
    
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); 
    glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);

    m_Shader->SetMat4("view", view);
    m_Shader->SetMat4("projection", projection);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    m_Shader->Unbind();
}

} // namespace Archura
