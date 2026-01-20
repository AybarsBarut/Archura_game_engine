#pragma once

#include <vector>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>

namespace Archura {

struct BoneInfo {
    int id;
    glm::mat4 offset;
};

struct KeyPosition {
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation {
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale {
    glm::vec3 scale;
    float timeStamp;
};

class Bone {
public:
public:
    // Making it generic:
    Bone(const std::string& name, int ID) : m_Name(name), m_ID(ID) {}
    
    // Keyframes
    std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;
    
    // Helpers to get interpolated transform
    glm::mat4 GetLocalTransform(float animationTime);
    
    std::string m_Name;
    int m_ID;
    glm::mat4 m_LocalTransform;
    
private:
    float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
    glm::mat4 InterpolatePosition(float animationTime);
    glm::mat4 InterpolateRotation(float animationTime);
    glm::mat4 InterpolateScale(float animationTime);
};

struct NodeData {
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<NodeData> children;
};

class Animation {
public:
    Animation() = default;
    Animation(const std::string& animationPath, class Model* model); // Loading path

    float m_Duration;
    int m_TicksPerSecond;
    NodeData m_RootNode;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    std::vector<Bone> m_Bones; // Store actual Bone objects with keyframes

    Bone* FindBone(const std::string& name) {
        auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
            [&](const Bone& bone) {
                return bone.m_Name == name;
            }
        );
        if (iter == m_Bones.end()) return nullptr;
        else return &(*iter);
    }
};

} // namespace Archura
