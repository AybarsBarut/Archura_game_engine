#include "Component.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Archura {

glm::mat4 Transform::GetModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    
    // Translate
    model = glm::translate(model, position);
    
    // Rotate (ZYX order)
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    
    // Scale
    model = glm::scale(model, scale);
    
    return model;
}

} // namespace Archura
