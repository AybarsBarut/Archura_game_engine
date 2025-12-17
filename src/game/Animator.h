#pragma once

#include "../ecs/Component.h"
#include "../rendering/Animation.h"
#include <vector>
#include <glm/glm.hpp>

namespace Archura {

class Animator : public Component {
public:
    Animator() = default;
    
    void PlayAnimation(Animation* pAnimation) {
        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;
    }

    void UpdateAnimation(float dt) {
        if (m_CurrentAnimation) {
            m_CurrentTime += m_CurrentAnimation->m_TicksPerSecond * dt;
            m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->m_Duration);
            CalculateBoneTransform(&m_CurrentAnimation->m_RootNode, glm::mat4(1.0f));
        }
    }

    void CalculateBoneTransform(const NodeData* node, glm::mat4 parentTransform) {
        std::string nodeName = node->name;
        glm::mat4 nodeTransform = node->transformation;

        Bone* Bone = FindBone(nodeName);

        if (Bone) {
            Bone->Update(m_CurrentTime); // Wait, Bone class in Animation.h didn't have Update logic exposed cleanly or I used GetLocalTransform?
            // "Bone" in Animation.h has GetLocalTransform.
            nodeTransform = Bone->GetLocalTransform(m_CurrentTime);
        }

        glm::mat4 globalTransformation = parentTransform * nodeTransform;

        auto boneInfoMap = m_CurrentAnimation->m_BoneInfoMap;
        if (boneInfoMap.find(nodeName) != boneInfoMap.end()) {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            
            if (index < 100) { // Max bones constraint
                m_FinalBoneMatrices[index] = globalTransformation * offset;
            }
        }

        for (int i = 0; i < node->childrenCount; i++) {
            CalculateBoneTransform(&node->children[i], globalTransformation);
        }
    }
    
    std::vector<glm::mat4> GetFinalBoneMatrices() { 
        return m_FinalBoneMatrices;
    }

private:
    Bone* FindBone(const std::string& name) {
        if (!m_CurrentAnimation) return nullptr;
        return m_CurrentAnimation->FindBone(name);
    }

    std::vector<glm::mat4> m_FinalBoneMatrices = std::vector<glm::mat4>(100, glm::mat4(1.0f));
    Animation* m_CurrentAnimation = nullptr;
    float m_CurrentTime = 0.0f;
};

} // namespace Archura
