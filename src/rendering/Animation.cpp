#include "Animation.h"
#include <ufbx.h>
#include <iostream>
#include <vector>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "../ecs/Entity.h" 

namespace Archura {

Animation::Animation(const std::string& animationPath, class Model* model) {
    ufbx_load_opts loadOpts = {0}; 
    ufbx_error error;
    ufbx_scene* scene = ufbx_load_file(animationPath.c_str(), &loadOpts, &error);
    if (!scene) {
        std::cerr << "Failed to load animation: " << animationPath << "\n";
        return;
    }

    if (scene->anim_stacks.count == 0) {
        ufbx_free_scene(scene);
        return;
    }

    // Select first stack
    ufbx_anim_stack* stack = scene->anim_stacks.data[0];
    
    // Create 'anim' for evaluation
    std::vector<uint32_t> layerIds;
    for (size_t i = 0; i < stack->layers.count; ++i) {
        layerIds.push_back(stack->layers.data[i]->typed_id);
    }
    
    ufbx_anim_opts animOpts = {}; 
    animOpts.layer_ids.data = layerIds.data();
    animOpts.layer_ids.count = layerIds.size();
    
    ufbx_anim* anim = ufbx_create_anim(scene, &animOpts, &error);
    if (!anim) {
         std::cerr << "Failed to create ufbx animation: " << error.description.data << "\n";
         ufbx_free_scene(scene);
         return;
    }

    m_TicksPerSecond = 30.0f; 
    m_Duration = (float)stack->time_end - (float)stack->time_begin;
    if(m_Duration <= 0.0f) m_Duration = 1.0f;

    int numFrames = (int)(m_Duration * m_TicksPerSecond);
    
    for (size_t i = 0; i < scene->nodes.count; i++) {
        ufbx_node* node = scene->nodes.data[i];
        std::string boneName = node->name.data;

        Bone bone(boneName, (int)i);

        for (int f = 0; f <= numFrames; f++) {
            double time = stack->time_begin + ((double)f / (double)m_TicksPerSecond);
            
            // Eval
            ufbx_transform transform = ufbx_evaluate_transform(anim, node, time);
            
            KeyPosition kp;
            kp.position = glm::vec3(transform.translation.x, transform.translation.y, transform.translation.z);
            kp.timeStamp = (float)time;
            bone.m_Positions.push_back(kp);
            
            KeyRotation kr;
            kr.orientation = glm::quat(transform.rotation.w, transform.rotation.x, transform.rotation.y, transform.rotation.z);
            kr.timeStamp = (float)time;
            bone.m_Rotations.push_back(kr);
            
            KeyScale ks;
            ks.scale = glm::vec3(transform.scale.x, transform.scale.y, transform.scale.z);
            ks.timeStamp = (float)time;
            bone.m_Scales.push_back(ks);
        }
        
        m_Bones.push_back(bone);
        
        BoneInfo info;
        info.id = (int)i;
        info.offset = glm::mat4(1.0f); 
        m_BoneInfoMap[boneName] = info;
    }

    ufbx_free_anim(anim);
    ufbx_free_scene(scene);
}

glm::mat4 Bone::GetLocalTransform(float animationTime) {
    glm::mat4 translation = InterpolatePosition(animationTime);
    glm::mat4 rotation = InterpolateRotation(animationTime);
    glm::mat4 scale = InterpolateScale(animationTime);
    return translation * rotation * scale;
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    // Avoid division by zero
    if (framesDiff < 0.0001f) return 0.0f;
    return midWayLength / framesDiff;
}

glm::mat4 Bone::InterpolatePosition(float animationTime) {
    if (1 == m_Positions.size())
        return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

    int p0Index = -1;
    for (int i = 0; i < m_Positions.size() - 1; ++i) {
        if (animationTime < m_Positions[i + 1].timeStamp) {
            p0Index = i;
            break;
        }
    }
    if (p0Index == -1) return glm::translate(glm::mat4(1.0f), m_Positions.back().position);

    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Positions[p0Index].timeStamp, m_Positions[p1Index].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(m_Positions[p0Index].position, m_Positions[p1Index].position, scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime) {
    if (1 == m_Rotations.size()) {
        auto rotation = glm::normalize(m_Rotations[0].orientation);
        return glm::mat4_cast(rotation);
    }

    int p0Index = -1;
    for (int i = 0; i < m_Rotations.size() - 1; ++i) {
        if (animationTime < m_Rotations[i + 1].timeStamp) {
            p0Index = i;
            break;
        }
    }
    if (p0Index == -1) return glm::mat4_cast(glm::normalize(m_Rotations.back().orientation));

    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Rotations[p0Index].timeStamp, m_Rotations[p1Index].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(m_Rotations[p0Index].orientation, m_Rotations[p1Index].orientation, scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::mat4_cast(finalRotation);
}

glm::mat4 Bone::InterpolateScale(float animationTime) {
    if (1 == m_Scales.size())
        return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

    int p0Index = -1;
    for (int i = 0; i < m_Scales.size() - 1; ++i) {
        if (animationTime < m_Scales[i + 1].timeStamp) {
            p0Index = i;
            break;
        }
    }
    if (p0Index == -1) return glm::scale(glm::mat4(1.0f), m_Scales.back().scale);

    int p1Index = p0Index + 1;
    float scaleFactor = GetScaleFactor(m_Scales[p0Index].timeStamp, m_Scales[p1Index].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(m_Scales[p0Index].scale, m_Scales[p1Index].scale, scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}

} // namespace Archura
