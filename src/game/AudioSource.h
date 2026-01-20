#pragma once

#include "../ecs/Component.h"
#include <string>

namespace Archura {

/**
 * @brief AudioSource component - Ses kaynagi ozellikleri
 */
struct AudioSource : public Component {
    std::string clipName;       // assets/audio/ folder relative path
    float volume = 1.0f;        // Base volume (0.0 - 1.0)
    bool loop = false;          // Should the audio loop?
    bool is3D = true;           // Is this a 3D generic sound?
    
    // 3D Settings
    float minDistance = 1.0f;   // Distance where volume is max
    float maxDistance = 20.0f;  // Distance where volume is 0 (linear rolloff)
    
    // Runtime internal state
    std::string runtimeAlias;   // Internal MCI alias
    bool isPlaying = false;
};

} // namespace Archura
