#pragma once

#include "../ecs/Component.h"

namespace Archura {

/**
 * @brief Lifetime component - Entities that self-destruct after a duration
 */
struct Lifetime : public Component {
    float remainingTime = 5.0f; // Seconds until destruction

    Lifetime(float time = 5.0f) : remainingTime(time) {}
};

} // namespace Archura
